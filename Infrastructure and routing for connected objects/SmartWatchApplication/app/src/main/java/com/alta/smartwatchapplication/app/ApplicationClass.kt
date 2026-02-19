package com.alta.altapfproject.app

import android.app.Application
import androidx.appcompat.app.AppCompatDelegate

import dagger.hilt.android.HiltAndroidApp


@HiltAndroidApp
//@GlideModule
class ApplicationClass : Application() {

    override fun onCreate() {
        super.onCreate()
        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO);

        // Optional: enable Crashlytics collection (default is automatic)
        // FireBaseCr.getInstance().setCrashlyticsCollectionEnabled(true)
    }
}
