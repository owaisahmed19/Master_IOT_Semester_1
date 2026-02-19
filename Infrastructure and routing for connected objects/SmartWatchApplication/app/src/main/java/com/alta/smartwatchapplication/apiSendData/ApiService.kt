package com.alta.smartwatchapplication.apiSendData

import retrofit2.http.Body
import retrofit2.http.POST

interface ApiService {
    @POST("locationSend")
    suspend fun sendLocation(@Body body: LocationData): retrofit2.Response<Unit>
}