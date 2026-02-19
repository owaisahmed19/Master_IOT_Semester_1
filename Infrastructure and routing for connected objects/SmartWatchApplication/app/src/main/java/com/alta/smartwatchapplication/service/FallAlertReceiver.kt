package com.alta.smartwatchapplication.service

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

class FallAlertReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == "ACTION_I_AM_OKAY") {
            // Send command back to Service to stop the siren
            val serviceIntent = Intent(context, BluetoothService::class.java).apply {
                action = BluetoothService.ACTION_STOP_SIREN
            }
            // Use startService (or startForegroundService) to deliver the intent
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                context.startForegroundService(serviceIntent)
            } else {
                context.startService(serviceIntent)
            }
        }
    }
}