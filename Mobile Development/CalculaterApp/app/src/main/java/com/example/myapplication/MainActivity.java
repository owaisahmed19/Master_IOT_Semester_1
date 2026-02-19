package com.example.myapplication;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    EditText edPriceI, edTex, edPriceF;
    Button btnCalculate;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        edPriceI = findViewById(R.id.edINPrice);
        edTex = findViewById(R.id.edTex);
        edPriceF = findViewById(R.id.edFullPrice);
        btnCalculate = findViewById(R.id.btnEnter);

        btnCalculate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                String priceText = edPriceI.getText().toString().trim();
                String texText = edTex.getText().toString().trim();

                if (!priceText.isEmpty() || !texText.isEmpty()) {
                    double price = Double.parseDouble(priceText);
                    double tex = Double.parseDouble(texText);
                    double finalPrice = price + (price * tex / 100.0);
                    edPriceF.setText(String.format("%.3f", finalPrice));
                } else {
                    Toast.makeText(MainActivity.this, "Price and Tex is required", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
}