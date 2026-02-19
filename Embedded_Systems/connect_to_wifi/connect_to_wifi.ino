

/*

part one
#include <ESP8266WiFi.h>

// --- WiFi credentials ---
const char* ssid     = "testingiot";
const char* password = "testingiot";

// --- Server configuration ---
const char* server_ip   = "192.168.0.2";
const int   server_port = 12345;

// --- Globals ---
WiFiClient client;
String clientId = "";   // received from server after connection

// --- Utility: read a line from TCP server ---
String readLine() {
  String line = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        break;  // line completed
      } else if (c != '\r') {
        line += c;
      }
    }
  }
  return line;
}

// --- Connect to WiFi ---
void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// --- Connect to TCP server and get clientId ---
bool connectServer() {
  Serial.print("Connecting to server ");
  Serial.print(server_ip);
  Serial.print(":");
  Serial.println(server_port);

  if (!client.connect(server_ip, server_port)) {
    Serial.println("Connection failed!");
    return false;
  }

  // Server sends client ID as first line
  clientId = readLine();
  Serial.print("Received client ID: ");
  Serial.println(clientId);

  return true;
}

// --- Send piezo value to server ---
void sendPiezoValue(int value) {
  if (!client.connected()) {
    Serial.println("Lost connection. Reconnecting...");
    if (!connectServer()) return;
  }

  // Format: PIEZO id_client piezo_value
  String request = "PIEZO " + clientId + " " + String(value) + "\n";
  client.print(request);

  // Wait for server response (1 or 2 lines)
  String response1 = readLine();
  Serial.println("Server response: " + response1);

  if (response1.startsWith("OK")) {
    String response2 = readLine();
    Serial.println("Voltage: " + response2);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  if (!connectServer()) {
    Serial.println("Initial connection to server failed. Will retry in loop.");
  }
}

void loop() {
  int piezoValue = analogRead(A0);

  if (piezoValue >= 500) {
    sendPiezoValue(piezoValue);
    delay(500);  // wait before next reading
  } else {
    delay(3);    // minimum delay between two analogRead on ESP8266
  }
}

*/


#include <ESP8266WiFi.h>

// ====== WiFi credentials ======
const char* ssid     = "testingiot";
const char* password = "testingiot";

// ====== TCP server parameters ======
const char* host = "192.168.0.2";
const uint16_t port = 12345;

// ====== Globals ======
WiFiClient client;
String clientId = "";          // Assigned by server
volatile bool mustSendRequest = false; // Set by ISR
volatile int buttonState = 0;  // Updated in ISR

// ====== Pins ======
const int buttonPin = D2; // GPIO4 on NodeMCU
const int ledPin    = D4; // Built-in LED (active LOW)

// =============================================================
// Function to read a line from the TCP server
// =============================================================
String readLine() {
  String line = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        break;
      }
      if (c != '\r') {  // ignore CR
        line += c;
      }
    }
  }
  return line;
}

// =============================================================
// WiFi connection
// =============================================================
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// =============================================================
// TCP server connection
// =============================================================
void connectToServer() {
  Serial.print("Connecting to server ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);

  while (!client.connect(host, port)) {
    Serial.println("Connection failed, retrying...");
    delay(1000);
  }

  Serial.println("Connected to server!");

  // First line received = client ID
  clientId = readLine();
  Serial.print("Client ID received: ");
  Serial.println(clientId);
}

// =============================================================
// Send button state to server
// =============================================================
void sendButtonState() {
  if (!client.connected()) {
    Serial.println("Lost connection to server, reconnecting...");
    connectToServer();
  }

  // Format: BUTTON id_client state
  String request = "BUTTON " + clientId + " " + String(buttonState) + "\n";
  client.print(request);
  Serial.print("Sent request: ");
  Serial.println(request);

  // Read server response
  String response = readLine();
  Serial.println("Response: " + response);
}

// =============================================================
// ISR for button
// =============================================================
void ICACHE_RAM_ATTR handleButtonInterrupt() {
  buttonState = digitalRead(buttonPin);

  // Control LED depending on button
  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH); // LED off (if active low)
  } else {
    digitalWrite(ledPin, LOW);  // LED on
  }

  mustSendRequest = true; // Tell loop() to send request
}

// =============================================================
// Setup
// =============================================================
void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP); // Button active LOW
  pinMode(ledPin, OUTPUT);

  connectToWiFi();
  connectToServer();

  // Attach interrupt on button pin, both rising and falling edges
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonInterrupt, CHANGE);
}

// =============================================================
// Loop
// =============================================================
void loop() {
  if (mustSendRequest) {
    sendButtonState();
    mustSendRequest = false; // Reset flag
  }
}

