package com.example.androidprojectfireimage.ui

import android.content.Intent
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import android.os.Bundle
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageCapture
import androidx.camera.core.ImageCaptureException
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import androidx.core.net.toUri
import androidx.lifecycle.lifecycleScope
import com.example.androidprojectfireimage.DEVICE_HEIGHT_METERS
import com.example.androidprojectfireimage.DISTANCE_BETWEEN_UPDATES
import com.example.androidprojectfireimage.FILE_FORMAT
import com.example.androidprojectfireimage.LOCATION_PERMISSION
import com.example.androidprojectfireimage.ObjFun.destinationPoint
import com.example.androidprojectfireimage.ObjFun.hasPermissions
import com.example.androidprojectfireimage.TIME_BETWEEN_UPDATES
import com.example.androidprojectfireimage.databinding.ActivityMainBinding
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File
import kotlin.math.abs
import kotlin.math.tan

class MainActivity : AppCompatActivity(), SensorEventListener, LocationListener {
    private var imageCapture: ImageCapture? = null
    var requestLocationPermission: ActivityResultLauncher<Array<String>>? = null
    private lateinit var sensorManager: SensorManager
    private val rotationMatrix = FloatArray(9)
    private val orientation = FloatArray(3)
    var locationManager: LocationManager? = null
    var mlocation: Location? = null
    private var azimuthDeg: Double = 0.0
    private var pitchDeg: Double = 0.0
    private val binding: ActivityMainBinding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }

    private fun locationChangeCheck() {
        if (locationManager == null) {
            locationManager = getSystemService(LOCATION_SERVICE) as LocationManager?
        }
        locationManager?.let { startLocationChangeRequests(it) }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(binding.root)
        sensorManager = getSystemService(SENSOR_SERVICE) as SensorManager

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

        if (hasPermissions(
                this@MainActivity,
                *LOCATION_PERMISSION
            )
        ) locationChangeCheck() else requestLocationPermission?.launch(LOCATION_PERMISSION)

        startCamera()
        startSensors()

        binding.captureButton.setOnClickListener {
            captureAndSend()
        }
    }

    private fun startSensors() {
        val rotSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
        if (rotSensor != null) {
            sensorManager.registerListener(this, rotSensor, SensorManager.SENSOR_DELAY_UI)
        } else {
            Toast.makeText(
                this, "Rotation sensor not available", Toast.LENGTH_SHORT
            ).show()
        }
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()

            imageCapture =
                ImageCapture.Builder().setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
                    .build()
            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(
                    this, CameraSelector.DEFAULT_BACK_CAMERA, Preview.Builder().build().also {
                        it.setSurfaceProvider(binding.preview.surfaceProvider)
                    }, imageCapture
                )
            } catch (exc: Exception) {
                exc.printStackTrace()
            }
        }, ContextCompat.getMainExecutor(this))
    }

    private fun captureAndSend() {
        val ic = imageCapture ?: return
        val file = File(cacheDir, FILE_FORMAT)
        val outputOptions = ImageCapture.OutputFileOptions.Builder(file).build()
        ic.takePicture(
            outputOptions,
            ContextCompat.getMainExecutor(this),
            object : ImageCapture.OnImageSavedCallback {
                override fun onError(exc: ImageCaptureException) {
                    exc.printStackTrace()
                }

                override fun onImageSaved(output: ImageCapture.OutputFileResults) {
                    computeAndSend()
                }
            })
    }

    override fun onResume() {
        super.onResume()
        val rotSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
        if (rotSensor != null) sensorManager.registerListener(
            this, rotSensor, SensorManager.SENSOR_DELAY_UI
        )
    }

    private fun computeAndSend() {
        val loc = mlocation
        if (loc == null) {
            lifecycleScope.launch(Dispatchers.Main) {
                Toast.makeText(this@MainActivity, "No location yet", Toast.LENGTH_SHORT).show()
            }
            return
        }

        val distanceMeters = abs(DEVICE_HEIGHT_METERS * tan(Math.toRadians(pitchDeg)))

        val (destLat, destLon) = destinationPoint(
            loc.latitude,
            loc.longitude,
            distanceMeters,
            azimuthDeg
        )

        val smsText = """
            Origin (Smartphone):
            Latitude = ${"%.6f".format(loc.latitude)}
            Longitude = ${"%.6f".format(loc.longitude)}
            Altitude = ${"%.1f".format(DEVICE_HEIGHT_METERS)} m
            Azimuth = ${"%.1f".format(azimuthDeg)}°
            Elevation = ${"%.1f".format(pitchDeg)}°
            Distance = ${"%.1f".format(distanceMeters)} m
            
            Smoke Point (Estimated):
            Latitude = ${"%.6f".format(destLat)}
            Longitude = ${"%.6f".format(destLon)}
        """.trimIndent()

        val smsIntent = Intent(Intent.ACTION_SENDTO, "smsto:".toUri()).apply {
            putExtra("sms_body", smsText)
        }
        startActivity(smsIntent)

    }

    override fun onLocationChanged(p0: Location) {
        setLocation(p0)
    }

    fun setLocation(mloc: Location) {
        mlocation = mloc
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
                    locationManager.getLastKnownLocation(networkProvider)?.let { setLocation(it) }
                }
                if (locationManager.isProviderEnabled(gpsProvider)) {
                    locationManager.requestLocationUpdates(
                        gpsProvider, TIME_BETWEEN_UPDATES, DISTANCE_BETWEEN_UPDATES, this
                    )
                    locationManager.getLastKnownLocation(gpsProvider)?.let {
                        setLocation(it)
                    }
                }
            } catch (ex: SecurityException) {
            }
        } catch (ex: Exception) {
        }
    }

    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {

    }

    override fun onPause() {
        super.onPause()
        sensorManager.unregisterListener(this)
    }

    override fun onSensorChanged(p0: SensorEvent) {
        if (p0.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
            SensorManager.getRotationMatrixFromVector(rotationMatrix, p0.values)
            SensorManager.getOrientation(rotationMatrix, orientation)
            val az = Math.toDegrees(orientation[0].toDouble())
            azimuthDeg = (az + 360) % 360.0
            pitchDeg = Math.toDegrees(orientation[1].toDouble())
            pitchDeg = pitchDeg.coerceIn(-89.0, 89.0)
        }
    }
}