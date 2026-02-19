package com.alta.airqualitycheck

import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.lifecycle.lifecycleScope
import com.alta.airqualitycheck.databinding.ActivityMainBinding
import kotlinx.coroutines.launch
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

class MainActivity : AppCompatActivity() {
    private val binding: ActivityMainBinding by lazy {
        ActivityMainBinding.inflate(layoutInflater)

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        enableEdgeToEdge()
        setContentView(binding.root)

        WindowCompat.setDecorFitsSystemWindows(window, false)
        window.statusBarColor = ContextCompat.getColor(this, R.color.primaryColor)

        ViewCompat.setOnApplyWindowInsetsListener(findViewById<View>(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(0, systemBars.top, 0, 0)
            insets
        }

        binding.btnRefresh.setOnClickListener {
            loadSensorData()
        }

        binding.btnOn.setOnClickListener {
            lifecycleScope.launch {
                try {
                    val response = RetrofitClient.api.sendLedCommand(LedCommand("ON"))
                    if (response.isSuccessful) {
                        Toast.makeText(this@MainActivity, "LED turned ON", Toast.LENGTH_SHORT)
                            .show()
                    } else {
                        Toast.makeText(
                            this@MainActivity,
                            "Failed to turn ON LED",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                } catch (e: Exception) {
                    Toast.makeText(this@MainActivity, "Error: ${e.message}", Toast.LENGTH_SHORT)
                        .show()
                }
            }
        }

        binding.btnOff.setOnClickListener {
            lifecycleScope.launch {
                try {
                    val response = RetrofitClient.api.sendLedCommand(LedCommand("OFF"))
                    if (response.isSuccessful) {
                        Toast.makeText(this@MainActivity, "LED turned OFF", Toast.LENGTH_SHORT)
                            .show()
                    } else {
                        Toast.makeText(
                            this@MainActivity,
                            "Failed to turn OFF LED",
                            Toast.LENGTH_SHORT
                        ).show()
                    }
                } catch (e: Exception) {
                    Toast.makeText(this@MainActivity, "Error: ${e.message}", Toast.LENGTH_SHORT)
                        .show()
                }
            }
        }


        loadSensorData()

    }


    private fun loadSensorData() {
        binding.apply {
            RetrofitClient.api.getSensorData()
                .enqueue(object : Callback<SensorData> {

                    override fun onResponse(
                        call: Call<SensorData>,
                        response: Response<SensorData>,
                    ) {
                        if (response.isSuccessful) {
                            val data = response.body() ?: return
                            Log.d("onResponse_______", "data: ${data}")

                            tvPm1.text = "PM1.0: ${data.pm1}"
                            tvPm25.text = "PM2.5: ${data.pm25}"
                            tvPm10.text = "PM10: ${data.pm10}"
                            tvTempC.text = "Temp (C): ${data.tempC}°C"
                            tvTempF.text = "Temp (F): ${data.tempF}°F"
                            tvHumidity.text = "Humidity: ${data.humidity}%"
                            tvAirQuality.text =
                                "Air Quality: ${data.airQualityText} (Level ${data.airQualityLevel})"
                        }
                    }

                    override fun onFailure(call: Call<SensorData>, t: Throwable) {
                        t.printStackTrace()
                        Log.d("onResponse_______", "error: ")
                        Log.d("onResponse_______", "t: $t")

                    }
                })
        }

    }
}