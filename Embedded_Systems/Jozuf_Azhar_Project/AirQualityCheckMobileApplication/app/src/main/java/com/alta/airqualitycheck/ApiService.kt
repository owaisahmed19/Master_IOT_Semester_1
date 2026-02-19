package com.alta.airqualitycheck

import retrofit2.Call
import retrofit2.Response
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.POST

interface ApiService {

    @GET("sensor")
    fun getSensorData(): Call<SensorData>

    @POST("led")
    suspend fun sendLedCommand(@Body body: LedCommand): Response<Unit>
}