
#include <ESP8266WiFi.h>

void setupWifi() {
     
  const char *ssid = "Owais's A32"; // the ssid of the AP        
  const char *password = "12211221";
  
  WiFi.setAutoConnect(false);  // see remark 1
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // always active => big consumption, see remark 2

  WiFi.mode (WIFI_STA); // setup as a wifi client
  WiFi.begin(ssid,password); // try to connect
  while (WiFi.status() != WL_CONNECTED) { // check connection
    delay(500); 
    Serial.print(".");
  } 

    Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
