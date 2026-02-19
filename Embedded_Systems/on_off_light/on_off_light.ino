#include <ESP8266WiFi.h>

const int buttonPin = D2;   // Switch pin
const int ledPin = D1;      // LED pin

bool ledState = false;      // Variable to store LED state (OFF at start)
bool lastButtonState = HIGH; // Remember last button state (assuming pull-up)

void setup() {
  Serial.begin(115200);
  delay(10);

  // Turn off WiFi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(500);

  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW); // LED off at start
}

void loop() {
  // Read button (active LOW, pressed = 0)
  bool currentButtonState = digitalRead(buttonPin);

  // Detect button press (transition from HIGH → LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    ledState = !ledState; // Toggle LED state
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    if (ledState) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }

    delay(200); // debounce delay
  }

  lastButtonState = currentButtonState; // save for next loop
}



