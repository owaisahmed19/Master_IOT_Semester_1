package com.alta.smartwatchapplication.utils.permission

import android.Manifest.permission.ACCESS_COARSE_LOCATION
import android.Manifest.permission.ACCESS_FINE_LOCATION
import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.provider.Settings
import androidx.annotation.RequiresApi

val LOCATION_PERMISSION = arrayOf(ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION)


val NOTIFICATION_PERMISSION = arrayOf(android.Manifest.permission.POST_NOTIFICATIONS)

fun Activity.openSettings(): Intent {
    return Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
        data = Uri.fromParts("package", packageName, null)
    }
}

fun Activity.getNotificationSetting(): Intent {
    return Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS).apply {
        putExtra(Settings.EXTRA_APP_PACKAGE, packageName)
    }
}

@RequiresApi(Build.VERSION_CODES.S)
fun Activity.getAlarmRemainderSetting(): Intent {
    return Intent(Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM)
}