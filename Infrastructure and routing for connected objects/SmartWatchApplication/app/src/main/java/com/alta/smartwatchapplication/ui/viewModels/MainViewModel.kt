package com.alta.smartwatchapplication.ui.viewModels

import android.content.Context
import android.location.Location
import android.widget.Toast
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.alta.smartwatchapplication.apiSendData.LocationData
import com.alta.smartwatchapplication.apiSendData.RetrofitClient
import com.alta.smartwatchapplication.repository.DataRepository
import com.alta.smartwatchapplication.utils.notifyObserver
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject


@HiltViewModel
class MainViewModel @Inject constructor(
    private val dataRepository: DataRepository,
) : ViewModel() {

    private val _locationViewModel = MutableLiveData<Location>()
    val locationViewModel: LiveData<Location>
        get() = _locationViewModel

    fun notifyChangeLocation() {
        _locationViewModel.notifyObserver()
    }

    fun setLocation(location: Location, context: Context) {
        dataRepository.setLocation(location)
        _locationViewModel.value = location

        viewModelScope.launch(Dispatchers.IO) {
        //    try {
                // val response =
                RetrofitClient.api.sendLocation(
                    LocationData(
                        System.currentTimeMillis(),
                        location.latitude,
                        location.longitude
                    )
                )
                /* if (response.isSuccessful) {
                     Toast.makeText(context, "", Toast.LENGTH_SHORT)
                         .show()
                 } else {
                     Toast.makeText(
                         context,
                         "Failed to turn ON LED",
                         Toast.LENGTH_SHORT
                     ).show()
                 }*/
           // } catch (e: Exception) {
            //    Toast.makeText(context, "Error: ${e.message}", Toast.LENGTH_SHORT)
           //         .show()
          //  }

        }
    }

}
