package com.alta.smartwatchapplication.ui.viewModels

import androidx.lifecycle.ViewModel
import com.alta.smartwatchapplication.repository.InternetConnectRepository
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject


@HiltViewModel
class InternetConnectViewModel @Inject constructor(
    private val dataRepository: InternetConnectRepository,
) : ViewModel() {

}
