package com.alta.smartwatchapplication.utils.permission

import android.content.Context
import android.content.pm.PackageManager
import androidx.core.app.ActivityCompat

object PermissionUtils {
    fun hasNotificationPermission(context: Context): Boolean =
        hasPermissions(
            context,
            *NOTIFICATION_PERMISSION
        )

    fun hasLocationPermission(context: Context): Boolean =
        hasPermissions(
            context,
            *LOCATION_PERMISSION
        )

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


}