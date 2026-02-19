package com.example.locationbasedapp

import android.Manifest.permission.ACCESS_COARSE_LOCATION
import android.Manifest.permission.ACCESS_FINE_LOCATION
import android.Manifest.permission.READ_SMS
import android.Manifest.permission.RECEIVE_SMS
import android.Manifest.permission.SEND_SMS
import android.content.Context
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.graphics.Color
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import android.os.Bundle
import android.telephony.SmsManager
import android.util.Log
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.example.locationbasedapp.databinding.ActivityMainBinding
import com.google.android.gms.maps.CameraUpdateFactory
import com.google.android.gms.maps.GoogleMap
import com.google.android.gms.maps.model.LatLng
import com.google.android.gms.maps.model.Marker
import com.google.android.gms.maps.model.MarkerOptions
import com.google.android.gms.maps.model.Polyline
import com.google.android.gms.maps.model.PolylineOptions
import java.util.jar.Manifest

class MainActivity : AppCompatActivity(), LocationListener {
    private val binding: ActivityMainBinding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }
    var locationManager: LocationManager? = null
    var previousMarker: Marker? = null
    var markerBelfort: Marker? = null
    var polyline: Polyline? = null

    val TIME_BETWEEN_UPDATES: Long = 5000
    val DISTANCE_BETWEEN_UPDATES = 500.0f

    private lateinit var smsReceiver: ReceiveSMS

    val latLonBelfort = LatLng(
        47.64,
        6.84
    )
    val LOCATION_PERMISSION = arrayOf(ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION)

    lateinit var mGoogleMap: GoogleMap
    var requestLocationPermission: ActivityResultLauncher<Array<String>>? = null

    private fun locationChangeCheck() {
        if (locationManager == null) {
            locationManager = getSystemService(LOCATION_SERVICE) as LocationManager?
        }
        locationManager?.let { startLocationChangeRequests(it) }
    }

    private fun setupSmsSender() {
        binding?.apply {


            buttonSendSms.setOnClickListener {
                val destinationAddress = editTextPhoneNumber.text.toString()
                val message = editTextMessage.text.toString()

                if (destinationAddress.isEmpty() || message.isEmpty()) {
                    Toast.makeText(
                        this@MainActivity,
                        "Please enter phone number and message.",
                        Toast.LENGTH_SHORT
                    ).show()
                    return@setOnClickListener
                }

                if (ContextCompat.checkSelfPermission(
                        this@MainActivity,
                        SEND_SMS
                    ) != PackageManager.PERMISSION_GRANTED
                ) {
                    ActivityCompat.requestPermissions(
                        this@MainActivity,
                        arrayOf(SEND_SMS),
                        102
                    )
                    return@setOnClickListener
                }

                try {
                    val smsManager = SmsManager.getDefault()
                    // For simplicity, using sendTextMessage directly. In production, use pending intents.
                    smsManager.sendTextMessage(destinationAddress, null, message, null, null)
                    Toast.makeText(this@MainActivity, "SMS sent to $destinationAddress.", Toast.LENGTH_LONG)
                        .show()
                } catch (e: Exception) {
                    Toast.makeText(this@MainActivity, "SMS failed to send. Error: ${e.message}", Toast.LENGTH_SHORT)
                        .show()
                    Log.e("MyMapView", "SMS send failed", e)
                }
            }
        }
    }



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)


        val permissions = arrayOf(
            RECEIVE_SMS,
            READ_SMS,
            SEND_SMS
        )

        val missingPermissions = permissions.filter {
            ContextCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missingPermissions.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, missingPermissions.toTypedArray(), 101)
        }

        // ✅ Register SMS receiver
        smsReceiver = ReceiveSMS()
        val filter = IntentFilter("android.provider.Telephony.SMS_RECEIVED")
        registerReceiver(smsReceiver, filter)
        // Setup SMS sending button for testing
        setupSmsSender()

        requestLocationPermission =
            registerForActivityResult(
                ActivityResultContracts.RequestMultiplePermissions()
            ) { permissions ->
                val allGranted = permissions.filter { !it.value }.keys
                if (!allGranted.isNotEmpty()) locationChangeCheck() else Toast.makeText(
                    this@MainActivity,
                    "Allow Location Permission to get Location Coordinates",
                    Toast.LENGTH_SHORT
                ).show()
            }

        binding.apply {
            mapView.onCreate(savedInstanceState)

            mapView.getMapAsync { googleMap ->
                mGoogleMap = googleMap
            }

            if (hasPermissions(
                    this@MainActivity,
                    *LOCATION_PERMISSION
                )
            ) locationChangeCheck() else requestLocationPermission?.launch(LOCATION_PERMISSION)
        }
    }


    fun hasPermissions(context: Context, vararg permissions: String): Boolean {
        if (permissions.isNotEmpty()) {
            for (permission in permissions) {
                if (ActivityCompat.checkSelfPermission(
                        context,
                        permission
                    ) != PackageManager.PERMISSION_GRANTED
                ) {
                    return false
                }
            }
        }
        return true
    }


    private fun startLocationChangeRequests(locationManager: LocationManager) {
        try {
            val networkProvider = LocationManager.NETWORK_PROVIDER
            val gpsProvider = LocationManager.GPS_PROVIDER
            try {
                if (locationManager.isProviderEnabled(networkProvider)) {
                    locationManager.requestLocationUpdates(
                        networkProvider, TIME_BETWEEN_UPDATES, DISTANCE_BETWEEN_UPDATES, this
                    )
                    locationManager.getLastKnownLocation(networkProvider)
                        ?.let { setLocation(it) }
                }
                if (locationManager.isProviderEnabled(gpsProvider)) {
                    locationManager.requestLocationUpdates(
                        gpsProvider, TIME_BETWEEN_UPDATES, DISTANCE_BETWEEN_UPDATES, this
                    )
                    locationManager.getLastKnownLocation(gpsProvider)
                        ?.let {
                            setLocation(it)
                        }
                }
            } catch (ex: SecurityException) {

            }
        } catch (ex: Exception) {

        }
    }

    fun setLocation(location: Location) {
        mGoogleMap.apply {
            val latLOn = LatLng(
                location.latitude,
                location.longitude
            )
            previousMarker?.remove()
            markerBelfort?.remove()

            previousMarker = addMarker(MarkerOptions().position(latLOn))
            markerBelfort = addMarker(MarkerOptions().position(latLonBelfort))

            polyline?.remove()
            polyline = null
            polyline = addPolyline(
                PolylineOptions()
                    .add(latLOn, latLonBelfort)
                    .width(4f)
                    .color(Color.BLUE)
                    .geodesic(true)
            )

            moveCamera(
                CameraUpdateFactory.newLatLngZoom(
                    latLOn, 10f
                )
            )
            animateCamera(
                CameraUpdateFactory.newLatLngZoom(
                    latLOn, 10f
                )
            )
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        // Unregister the receiver to prevent leaks
        unregisterReceiver(smsReceiver)
        binding.mapView.onDestroy()

        Log.d("MyMapView", "ReceiveSMS BroadcastReceiver unregistered.")
    }

    override fun onLocationChanged(p0: Location) {
        setLocation(p0)
    }


}

