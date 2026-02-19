
package com.alta.smartwatchapplication.service

import android.Manifest.permission.ACCESS_FINE_LOCATION
import android.Manifest.permission.SEND_SMS
import android.annotation.SuppressLint
import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.bluetooth.*
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.media.MediaPlayer
import android.os.Build
import android.os.Handler
import android.os.IBinder
import android.os.Looper
import android.telephony.SmsManager
import android.util.Log
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import com.alta.smartwatchapplication.R
import com.alta.smartwatchapplication.repository.DataRepository
import com.alta.smartwatchapplication.utils.EMERGENCY_PHONE_NUMBER
import com.alta.smartwatchapplication.utils.charUUID
import com.alta.smartwatchapplication.utils.serviceUUID
import dagger.hilt.android.AndroidEntryPoint
import java.util.*
import javax.inject.Inject

@AndroidEntryPoint
class BluetoothService : Service() {

    @Inject
    lateinit var dataRepository: DataRepository

    // Bluetooth Variables
    private var bluetoothManager: BluetoothManager? = null
    private var bluetoothAdapter: BluetoothAdapter? = null
    // No need to store bluetoothGatt globally unless you plan to close it elsewhere, 
    // but useful for debugging or disconnecting manually.
    private var bluetoothGatt: BluetoothGatt? = null

    // Alert Variables
    private var mediaPlayer: MediaPlayer? = null
    private var isAlertActive = false // <--- Critical Flag

    // Logic Handler (Main Thread)
    private val handler = Handler(Looper.getMainLooper())
    private val fallTimeoutRunnable = Runnable { finalizeEmergency() }

    companion object {
        const val CHANNEL_ID = "FALL_DETECTION_CHANNEL"
        const val NOTIFICATION_ID = 1
        const val ACTION_STOP_SIREN = "STOP_FALL_SEQUENCE"
    }

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel()
        bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager?.adapter
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val action = intent?.action

        if (action == ACTION_STOP_SIREN) {
            stopSirenAndReset()
            Toast.makeText(this, "Glad you are okay!", Toast.LENGTH_SHORT).show()

        } else {
            // Default start
            val notification = createNotification("Monitoring for falls...")
            startForeground(NOTIFICATION_ID, notification)
            startBleScan()
        }
        return START_STICKY
    }

    // ----------------------------------------------------------------
    // 1. EMERGENCY TRIGGER LOGIC
    // ----------------------------------------------------------------
    private fun triggerEmergencyFlow() {
        // Prevent double alerts
        if (isAlertActive) return

        isAlertActive = true
        Log.d("FALL_SERVICE", "Fall Detected! Starting Siren.")

        // 1. Play Siren
        playSiren()

        // 2. Update Notification
        val notification = createEmergencyNotification()
        val manager = getSystemService(NotificationManager::class.java)
        manager.notify(NOTIFICATION_ID, notification)

        // 3. Start 30s Countdown
        handler.postDelayed(fallTimeoutRunnable, 30000)
    }

    // ----------------------------------------------------------------
    // 2. STOP & RESET LOGIC
    // ----------------------------------------------------------------
    private fun stopSirenAndReset() {
        Log.d("FALL_SERVICE", "Stopping Siren and Resetting.")

        // 1. Stop Audio
        try {
            if (mediaPlayer != null) {
                if (mediaPlayer!!.isPlaying) {
                    mediaPlayer?.stop()
                }
                mediaPlayer?.release()
                mediaPlayer = null
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }

        // 2. Stop Countdown
        handler.removeCallbacks(fallTimeoutRunnable)

        // 3. Reset Flag (IMPORTANT)
        isAlertActive = false

        // 4. Update Notification back to Normal
        val notification = createNotification("Monitoring for falls... (User Safe)")
        val manager = getSystemService(NotificationManager::class.java)
        manager.notify(NOTIFICATION_ID, notification)

        // Ensure foreground state is maintained
        startForeground(NOTIFICATION_ID, notification)

    }

    private fun finalizeEmergency() {
        if (isAlertActive) {
            Log.d("FALL_SERVICE", "Timeout! Sending SMS.")

            Toast.makeText(this, "Sending Emergency SMS!", Toast.LENGTH_LONG).show()

            // Send the SMS with Location
            sendSmsWithLocation()

            // Finally stop the siren
            stopSirenAndReset()
        }
    }

    @SuppressLint("MissingPermission")
    private fun sendSmsWithLocation() {
        // Check SMS Permission
        if (ActivityCompat.checkSelfPermission(this, SEND_SMS) != PackageManager.PERMISSION_GRANTED) {
            Log.e("FALL_SERVICE", "SMS Permission NOT granted!")
            Toast.makeText(this, "ERROR: Cannot send SMS (No Permission)", Toast.LENGTH_LONG).show()
            return
        }

        // Check Location Permission
        if (ActivityCompat.checkSelfPermission(this, ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            Log.w("FALL_SERVICE", "Location Permission missing, sending generic SMS.")
            sendActualSms("ALERT! Fall detected! Location unavailable.")
            return
        }

        val location = dataRepository.getLocation()

        if (location != null) {
            // FIXED: Added '$' so the numbers actually appear in the link
            // Using a standard Google Maps query format
            val mapsLink = "https://www.google.com/maps/search/?api=1&query=${location.latitude},${location.longitude}"
            val message = "EMERGENCY: Fall Detected! Help needed. Loc: $mapsLink"
            sendActualSms(message)
        } else {
            sendActualSms("EMERGENCY: Fall Detected! Help needed. (Location unavailable)")
        }
    }

    private fun sendActualSms(message: String) {
        try {
            val smsManager = getSystemService(SmsManager::class.java)

            // Use standard SMS manager
            smsManager.sendTextMessage(EMERGENCY_PHONE_NUMBER, null, message, null, null)

            Handler(Looper.getMainLooper()).post {
                Toast.makeText(this, "SOS Sent to $EMERGENCY_PHONE_NUMBER", Toast.LENGTH_LONG).show()
            }
            Log.d("FALL_SERVICE", "SMS Sent: $message")

        } catch (e: Exception) {
            Log.e("FALL_SERVICE", "SMS Failed: ${e.message}")
            e.printStackTrace()
        }
    }

    private fun playSiren() {
        try {
            mediaPlayer?.release()
            mediaPlayer = MediaPlayer.create(this, R.raw.alert)
            mediaPlayer?.isLooping = true
            mediaPlayer?.start()
        } catch (e: Exception) {
            Log.e("FALL_SERVICE", "Error playing sound: ${e.message}")
        }
    }

    // ----------------------------------------------------------------
    // BLE LOGIC
    // ----------------------------------------------------------------

    @SuppressLint("MissingPermission")
    private fun startBleScan() {
        val scanner = bluetoothAdapter?.bluetoothLeScanner ?: return

        // Scan for everything (Low Latency) to ensure we find the device quickly
        val scanSettings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()

        Log.d("BLE_DEBUG", "Starting scan for ALL devices...")
        scanner.startScan(null, scanSettings, scanCallback)
    }

    private val scanCallback = object : ScanCallback() {
        @SuppressLint("MissingPermission")
        override fun onScanResult(callbackType: Int, result: ScanResult?) {
            val device = result?.device ?: return
            val name = device.name ?: "Unknown"
            val address = device.address

            Log.d("BLE_DEBUG", "Found: $name [$address]")

            // ADD THE NEW ADDRESS FROM YOUR LOGS HERE:
            if (address == "18:03:13:E8:0F:E0" || address == "36:08:8A:53:D1:36" || name == "CareWatch-IoT") {
                Log.d("BLE_DEBUG", "MATCH FOUND! Connecting to $address")
                bluetoothAdapter?.bluetoothLeScanner?.stopScan(this)
                bluetoothGatt = device.connectGatt(this@BluetoothService, false, gattCallback)
            }
        }
    }

    private val gattCallback = object : BluetoothGattCallback() {
        @SuppressLint("MissingPermission")
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d("BLE_DEBUG", "Connected to GATT server.")

                // IMPORTANT FIX: Reset the flag on connection so falls are detected again
                isAlertActive = false

                gatt.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.d("BLE_DEBUG", "Disconnected from GATT server.")
                isAlertActive = false
            }
        }

        @SuppressLint("MissingPermission")
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val service = gatt.getService(serviceUUID)
                val characteristic = service?.getCharacteristic(charUUID)

                if (characteristic != null) {
                    // 1. Enable notifications locally
                    gatt.setCharacteristicNotification(characteristic, true)

                    // 2. DELAY before writing descriptor (Critical for Android 12+)
                    handler.postDelayed({
                        val descriptor = characteristic.getDescriptor(UUID.fromString("00002902-0000-1000-8000-00805f9b34fb"))
                        if (descriptor != null) {
                            descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                            gatt.writeDescriptor(descriptor)
                            Log.d("BLE_DEBUG", "Descriptor written - Listening for FALL alerts")
                        }
                    }, 200)
                }
            }
        }

        override fun onCharacteristicChanged(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic) {
            // Read the value carefully
            val data = characteristic.value ?: return
            val message = String(data).trim().uppercase()

            Log.d("BLE_DEBUG", "Received message: $message")

            if (message.contains("FALL")) {
                Log.d("BLE_DEBUG", "FALL confirmed! Triggering Emergency UI.")
                handler.post { triggerEmergencyFlow() }
            }
        }
    }

    // ----------------------------------------------------------------
    // NOTIFICATION HELPERS
    // ----------------------------------------------------------------
    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(CHANNEL_ID, "Fall Service", NotificationManager.IMPORTANCE_HIGH)
            getSystemService(NotificationManager::class.java).createNotificationChannel(channel)
        }
    }

    private fun createNotification(text: String): Notification {
        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("Fall Guard")
            .setContentText(text)
            .setSmallIcon(R.drawable.ic_launcher_background)
            .setOngoing(true)
            .build()
    }

    private fun createEmergencyNotification(): Notification {
        val okIntent = Intent(this, FallAlertReceiver::class.java).apply { action = "ACTION_I_AM_OKAY" }
        val okPendingIntent = PendingIntent.getBroadcast(
            this, 0, okIntent, PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )

        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("FALL DETECTED!")
            .setContentText("Tap I AM OKAY to stop alarm")
            .setSmallIcon(R.drawable.ic_launcher_background)
            .addAction(R.drawable.ic_launcher_background, "I AM OKAY", okPendingIntent)
            .setPriority(NotificationCompat.PRIORITY_MAX)
            .setOngoing(false) // Allow user to dismiss if needed, though button is preferred
            .build()
    }

    override fun onBind(intent: Intent?): IBinder? = null
}

