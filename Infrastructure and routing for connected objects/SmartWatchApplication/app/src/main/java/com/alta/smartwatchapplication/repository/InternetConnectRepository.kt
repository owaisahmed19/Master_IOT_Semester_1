package com.alta.smartwatchapplication.repository

import android.content.Context
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import androidx.lifecycle.MutableLiveData
import javax.inject.Inject

class InternetConnectRepository @Inject constructor(
    val applicationClass: Context,
) {
    private val connectivityManager =
        applicationClass.getSystemService(ConnectivityManager::class.java)

    val _isConnected = MutableLiveData<Boolean>()

    private val callback = object : ConnectivityManager.NetworkCallback() {
        override fun onCapabilitiesChanged(
            network: Network,
            networkCapabilities: NetworkCapabilities
        ) {
            val connected = networkCapabilities.hasCapability(
                NetworkCapabilities.NET_CAPABILITY_VALIDATED
            )
            _isConnected.postValue(connected)
        }

        override fun onAvailable(network: Network) {
            _isConnected.postValue(true)
        }

        override fun onUnavailable() {
            _isConnected.postValue(false)

        }

        override fun onLost(network: Network) {
            _isConnected.postValue(false)
        }

    }

    private var isInitialized = false

    fun initNetworkObserver() {
        if (isInitialized) return
        isInitialized = true
        val capabilities = connectivityManager.getNetworkCapabilities(connectivityManager.activeNetwork)
        _isConnected.value =
            capabilities?.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED) == true

        connectivityManager.registerDefaultNetworkCallback(callback)
    }


    fun onClearedCalled() {
        if (isInitialized) {
            connectivityManager.unregisterNetworkCallback(callback)
            isInitialized = false
        }
    }
}