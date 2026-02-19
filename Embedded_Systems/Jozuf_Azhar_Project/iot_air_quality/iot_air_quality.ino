#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Seeed_HM330X.h>
#include "DHT.h"

// ------------------- PINS --------------------
#define SDA_PIN 22
#define SCL_PIN 23
#define BUZZER_PIN 18
#define DHTPIN 19
#define DHTTYPE DHT11
#define LED_PIN_ESP 2
#define LED_PIN 21   // LED pin

// ------------------- OBJECTS --------------------
DHT dht(DHTPIN, DHTTYPE);
HM330X pmSensor;
uint16_t pmValues[3]; // PM1.0, PM2.5, PM10

// ------------------- WIFI --------------------
const char* ssid = "OwaisA32";
const char* password = "12211221";

// ------------------- MQTT --------------------
const char* mqtt_server = "10.160.115.194";
const int mqtt_port = 1883;

const char* pm1_topic      = "esp32/pm1";
const char* pm25_topic     = "esp32/pm25";
const char* pm10_topic     = "esp32/pm10";
const char* tempC_topic    = "esp32/temperatureC";
const char* tempF_topic    = "esp32/temperatureF";
const char* humidity_topic = "esp32/humidity";
const char* airQualityLevel_topic = "esp32/airQualityLevel";
const char* client_id = "ESP32_AirSensor";
const char* led_topic = "esp32/ledgreen";  // Updated topic
const char* led_topic_esp = "esp32/led";
WiFiClient espClient;
PubSubClient client(espClient);

// ------------------- TIMING --------------------
unsigned long lastMsg = 0;
const long interval = 5000;  // 5 seconds

// ------------------- FUNCTIONS --------------------
void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(client_id)) {
      Serial.println("connected");
      client.subscribe(led_topic); // subscribe to LED control
      client.subscribe(led_topic_esp); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Callback for MQTT messages (LED control)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == led_topic) {
    Serial.print("MQTT LED control received: ");
    Serial.println(msg);
    if (msg == "ON") digitalWrite(LED_PIN, HIGH);
    else if (msg == "OFF") digitalWrite(LED_PIN, LOW);
  }

 if (String(topic) == led_topic_esp) {
    Serial.print("ESP32 LED CMD: ");
    Serial.println(msg);

    if (msg == "ON") digitalWrite(LED_PIN_ESP, HIGH);
    else if (msg == "OFF") digitalWrite(LED_PIN_ESP, LOW);
  }
}

// Read PM sensor
void readPM() {
  uint8_t buf[30];
  if (pmSensor.read_sensor_value(buf, 29) != NO_ERROR) {
    pmValues[0] = pmValues[1] = pmValues[2] = 0;
    return;
  }

  pmValues[0] = (buf[4] << 8) | buf[5];  // PM1.0
  pmValues[1] = (buf[6] << 8) | buf[7];  // PM2.5
  pmValues[2] = (buf[8] << 8) | buf[9];  // PM10
}

// Publish all sensor data
void publishData(float tempC, float tempF, float hum) {
  char buf[16];

  sprintf(buf, "%u", pmValues[0]); client.publish(pm1_topic, buf);
  sprintf(buf, "%u", pmValues[1]); client.publish(pm25_topic, buf);
  sprintf(buf, "%u", pmValues[2]); client.publish(pm10_topic, buf);

  dtostrf(tempC, 1, 2, buf); client.publish(tempC_topic, buf);
  dtostrf(tempF, 1, 2, buf); client.publish(tempF_topic, buf);
  dtostrf(hum, 1, 2, buf); client.publish(humidity_topic, buf);

  // Air quality level based on PM2.5
  int airQualityLevel = 1;

if (pmValues[1] <= 12) {
  airQualityLevel = 1;
  digitalWrite(BUZZER_PIN, LOW);   // OFF
}
else if (pmValues[1] <= 35) {
  airQualityLevel = 2;
  digitalWrite(BUZZER_PIN, LOW);   // OFF
}
else if (pmValues[1] <= 55) {
  airQualityLevel = 3;
  digitalWrite(BUZZER_PIN, HIGH);  // ON
}
else {
  airQualityLevel = 4;
  digitalWrite(BUZZER_PIN, HIGH);  // ON
}

  sprintf(buf, "%d", airQualityLevel);
  client.publish(airQualityLevel_topic, buf);
}

// ------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED initially OFF


pinMode(LED_PIN_ESP, OUTPUT);
  digitalWrite(LED_PIN_ESP, LOW);  // LED initially OFF
  

pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);

  dht.begin();

  if (pmSensor.init()) Serial.println("HM330X init failed");
  else Serial.println("HM330X ready");

  delay(3000);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

// ------------------- LOOP --------------------
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

bool smokeDetected = false;

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    readPM();
    float hum   = dht.readHumidity();
    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);

    Serial.println("------------");
    Serial.print("PM1.0: "); Serial.println(pmValues[0]);
    Serial.print("PM2.5: "); Serial.println(pmValues[1]);
    Serial.print("PM10 : "); Serial.println(pmValues[2]);
    Serial.print("TempC: "); Serial.println(tempC);
    Serial.print("Hum  : "); Serial.println(hum);

    publishData(tempC, tempF, hum);
  }
}
