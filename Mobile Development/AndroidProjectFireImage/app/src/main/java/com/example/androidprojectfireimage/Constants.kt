package com.example.androidprojectfireimage

import android.Manifest.permission.ACCESS_COARSE_LOCATION
import android.Manifest.permission.ACCESS_FINE_LOCATION
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

const val DEVICE_HEIGHT_METERS = 1.5
const val TIME_BETWEEN_UPDATES: Long = 5000
const val DISTANCE_BETWEEN_UPDATES = 500.0f

const val EARTH_RADIUS_METERS = 6378137.04

val LOCATION_PERMISSION = arrayOf(ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION)
val FILE_FORMAT = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(Date()) + ".jpg"
