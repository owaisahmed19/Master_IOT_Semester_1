package com.alta.airqualitycheck



data class SensorData(
    val pm1: Int,
    val pm25: Int,
    val pm10: Int,
    val tempC: Double,
    val tempF: Double,
    val humidity: Double,
    val airQualityLevel: Int,
    val airQualityText: String
)
