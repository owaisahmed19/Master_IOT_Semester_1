package com.alta.smartwatchapplication.utils

import android.app.Activity
import android.content.Context
import android.content.res.ColorStateList
import android.graphics.Color
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.util.Patterns
import android.view.View
import android.widget.EditText
import android.widget.Toast
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.MutableLiveData
import com.alta.smartwatchapplication.R

fun Fragment.contextActivity(callback: (Activity) -> Unit) {
    activity?.takeIf { isAdded && !isDetached }?.activityIsAlive(callback)
}

fun Activity.activityIsAlive(callback: (Activity) -> Unit) {
    if (!isFinishing && !isDestroyed) callback(this)
}

fun View.setBackgroundTint(color: Int) {
    backgroundTintList = ColorStateList.valueOf(color)
}

fun View.setBGColor(color: String) {
    setBackgroundColor(Color.parseColor(color))
}

fun Activity.setStatusBarColor(statusBarColorRes: Int = R.color.white) {
    val window = this.window
    window.statusBarColor = ContextCompat.getColor(this, statusBarColorRes)
    window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
}

fun View.setDelayClickListener(delay: Long = CLICK_DELAY, onClick: (View) -> Unit) {

    var isClickEnabled = true
    setOnClickListener {
        if (isClickEnabled) {
            isClickEnabled = false
            onClick(it)
            postDelayed({ isClickEnabled = true }, delay)
        }
    }
}


fun View.setVisibilityGone() {
    visibility = View.GONE
}

fun View.setVisibilityVisible() {
    visibility = View.VISIBLE
}

fun View.setVisibilityInVisible() {
    visibility = View.INVISIBLE
}


fun <T> MutableLiveData<T>.notifyObserver() {
    this.value = this.value
}

fun Context.showToast(message: String, duration: Int = Toast.LENGTH_SHORT) {
    Toast.makeText(this, message, duration).show()
}

fun EditText.isEmptyWithError(errorMessage: String = "Required!"): Boolean {
    val empty = this.text.toString().trim().isEmpty()
    if (empty) {
        this.error = errorMessage
        this.requestFocus()
    }
    return empty
}

val namePattern = "^[a-zA-Z]+$".toRegex()

fun EditText.validateEmail(errorMessage: String = "Enter valid email"): Boolean {
    val email = this.text.toString().trim()
    val isValid = Patterns.EMAIL_ADDRESS.matcher(email).matches()
    if (!isValid) {
        this.error = errorMessage
        this.requestFocus()
    }
    return isValid
}



fun Context.isInternetAvailable(): Boolean {
    val connectivityManager = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

    val network = connectivityManager.activeNetwork ?: return false
    val activeNetwork = connectivityManager.getNetworkCapabilities(network) ?: return false
    return when {
        activeNetwork.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) -> true
        activeNetwork.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) -> true
        activeNetwork.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) -> true
        else -> false
    }
}



