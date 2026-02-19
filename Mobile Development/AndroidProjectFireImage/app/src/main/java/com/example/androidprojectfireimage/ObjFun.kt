package com.example.androidprojectfireimage

import android.content.Context
import android.content.pm.PackageManager
import androidx.core.app.ActivityCompat
import kotlin.math.asin
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.sin

object ObjFun {
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

    fun destinationPoint(
        lat: Double,
        lon: Double,
        distance: Double,
        azimuth: Double,
    ): Pair<Double, Double> {

        val latRad = Math.toRadians(lat)
        val lonRad = Math.toRadians(lon)
        val azimuthRad = Math.toRadians(azimuth)
        val destLat = asin(
            sin(latRad) * cos(distance / EARTH_RADIUS_METERS) +
                    cos(latRad) * sin(distance / EARTH_RADIUS_METERS) * cos(azimuthRad)
        )
        val destLon = lonRad + atan2(
            sin(azimuthRad) * sin(distance / EARTH_RADIUS_METERS) * cos(latRad),
            cos(distance / EARTH_RADIUS_METERS) - sin(latRad) * sin(destLat)
        )
        return Pair(Math.toDegrees(destLat), Math.toDegrees(destLon))
    }


}