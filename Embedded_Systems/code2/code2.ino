#include <ESP8266WiFi.h>

int buttonPin = D2;   
int ledPin = D1;      
int buttonState = 0;  
int lastState = 0;   
int ledState = LOW;   

void setup() {

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10); 

  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastState == HIGH) {
  
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    if (ledState == HIGH) {
      Serial.println("Light ON");
    } else {
      Serial.println("Light OFF");
    }

    delay(50); 
  }

  lastState = buttonState;
}
