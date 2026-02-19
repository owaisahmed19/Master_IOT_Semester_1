package com.alta.smartwatchapplication.ui.activity

import android.Manifest
import android.location.Location
import android.location.LocationManager
import android.os.Build
import android.os.Bundle
import android.view.View
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.annotation.RequiresPermission
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.navigation.NavController
import androidx.navigation.fragment.NavHostFragment
import com.alta.smartwatchapplication.R
import com.alta.smartwatchapplication.databinding.ActivityMainBinding
import com.alta.smartwatchapplication.ui.viewModels.MainViewModel
import com.alta.smartwatchapplication.utils.AppObjs.isGPSEnabled
import com.alta.smartwatchapplication.utils.DISTANCE_BETWEEN_UPDATES
import com.alta.smartwatchapplication.utils.TIME_BETWEEN_UPDATES
import com.alta.smartwatchapplication.utils.permission.PermissionUtils.hasLocationPermission
import dagger.hilt.android.AndroidEntryPoint

@AndroidEntryPoint
class MainActivity : AppCompatActivity(), android.location.LocationListener {

    var locationManager: LocationManager? = null
    private lateinit var navController: NavController
    private val appViewModel: MainViewModel by viewModels()
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
        /*ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }*/

        if (hasLocationPermission(this) && isGPSEnabled(this)) {
            locationManager = getSystemService(LOCATION_SERVICE) as LocationManager?
            locationManager?.let { startLocationChangeRequests(it) }
        }



        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            enableEdgeToEdge()
        }

        /* appViewModel.locationViewModel.observe(this) {
             locationChangeCheck()
         }*/


        WindowCompat.setDecorFitsSystemWindows(window, false)

        window.statusBarColor = ContextCompat.getColor(this, R.color.primaryColor) // set your color

        ViewCompat.setOnApplyWindowInsetsListener(findViewById<View>(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(0, systemBars.top, 0, 0) // top padding for status bar only
            insets
        }


        val navHostFragment =
            supportFragmentManager.findFragmentById(R.id.container) as NavHostFragment
        navController = navHostFragment.navController

    }


    override fun onLocationChanged(p0: Location) {
        appViewModel.setLocation(p0, this@MainActivity)
    }

    override fun onSupportNavigateUp(): Boolean {
        return navController.navigateUp() || super.onSupportNavigateUp()
    }


    private fun startLocationChangeRequests(locationManager: LocationManager) {
        try {
            val networkProvider = LocationManager.NETWORK_PROVIDER
            val gpsProvider = LocationManager.GPS_PROVIDER

            // 1. Check Network Provider
            try {
                if (locationManager.isProviderEnabled(networkProvider)) {
                    locationManager.requestLocationUpdates(
                        networkProvider,
                        TIME_BETWEEN_UPDATES,
                        DISTANCE_BETWEEN_UPDATES,
                        this
                    )
                    locationManager.getLastKnownLocation(networkProvider)
                        ?.let { appViewModel.setLocation(it, this@MainActivity) }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }

            // 2. Check GPS Provider
            try {
                if (locationManager.isProviderEnabled(gpsProvider)) {
                    locationManager.requestLocationUpdates(
                        gpsProvider,
                        TIME_BETWEEN_UPDATES,
                        DISTANCE_BETWEEN_UPDATES,
                        this
                    )
                    locationManager.getLastKnownLocation(gpsProvider)
                        ?.let { appViewModel.setLocation(it, this@MainActivity) }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }

        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }

}