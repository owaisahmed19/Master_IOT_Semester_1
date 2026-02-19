package com.alta.smartwatchapplication.repository

import android.content.Context
import android.location.Location
import javax.inject.Inject


class DataRepository @Inject constructor(
    val applicationClass: Context,
) {


    private var location: Location? = null // Private backing field

    fun getLocation(): Location? {
        return location
    }

    fun setLocation(l0: Location) {
        location = l0
    }
}