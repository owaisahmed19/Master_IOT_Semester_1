#include "config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

TTGOClass *watch;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      // Allow for more stable connection with Android
      BLEDevice::setMTU(512); 
      Serial.println("Android Connected");
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Android Disconnected - Restarting Advertising");
      pServer->getAdvertising()->start();
    }
};

void setup() {
    watch = TTGOClass::getWatch();
    watch->begin();
    watch->openBL();
    Serial.begin(115200);

    watch->bma->begin();
    watch->bma->enableAccel();

    BLEDevice::init("CareWatch-IoT");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    
    // This Descriptor is mandatory for Android Notify to work
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
    
    watch->tft->fillScreen(TFT_BLACK);
    watch->tft->setTextColor(TFT_WHITE);
    watch->tft->drawString("Guard Active", 45, 100, 2);
    Serial.println("Watch ready...");
}

void loop() {
    Accel acc;
    static unsigned long lastNotifyTime = 0;
    bool fallDetected = false;

    if (watch->bma->getAccel(acc)) {
        float force = sqrt(sq(acc.x) + sq(acc.y) + sq(acc.z));

        // Logic: Free fall (low force) followed by Impact (high force)
        if (force < 450) { 
            unsigned long startFall = millis();
            while (millis() - startFall < 500) {
                if (watch->bma->getAccel(acc)) {
                    force = sqrt(sq(acc.x) + sq(acc.y) + sq(acc.z));
                    if (force > 2500) { 
                        fallDetected = true;
                        break;
                    }
                }
            }
        }
    }

    if (fallDetected || (millis() - lastNotifyTime > 5000)) {
        if (deviceConnected) {
            String payload = fallDetected ? "FALL" : "OK";
            pCharacteristic->setValue(payload.c_str());
            pCharacteristic->notify();
            Serial.println("BLE Sent: " + payload);
        }
        lastNotifyTime = millis();

        if (fallDetected) {
            watch->tft->fillScreen(TFT_RED);
            watch->tft->drawString("FALL DETECTED", 40, 100, 2);
            delay(3000); 
            watch->tft->fillScreen(TFT_BLACK);
            watch->tft->drawString("Guard Active", 45, 100, 2);
        }
    }
    delay(10); // Faster loop
}