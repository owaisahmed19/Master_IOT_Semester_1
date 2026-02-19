package com.alta.smartwatchapplication.utils

import android.Manifest
import android.app.Activity
import android.content.Context.LOCATION_SERVICE
import android.location.LocationManager

object AppObjs {
    fun myDelayClick(callback: () -> Unit) {
        val currentTime = System.currentTimeMillis()
        if (currentTime - LAST_CLICK_TIME >= CLICK_DELAY) {
            LAST_CLICK_TIME = currentTime
            callback.invoke()
        }
    }

    val perms = arrayOf(
        Manifest.permission.BLUETOOTH_SCAN,
        Manifest.permission.BLUETOOTH_CONNECT,
        Manifest.permission.ACCESS_FINE_LOCATION
    )

    private lateinit var locationManager: LocationManager

    fun isGPSEnabled(activity: Activity): Boolean {
        locationManager = activity.getSystemService(LOCATION_SERVICE) as LocationManager
        return locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
    }
}