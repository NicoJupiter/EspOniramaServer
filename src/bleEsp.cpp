#include "bleEsp.h"
#include <BLEDevice.h>
#include <BLEServer.h>

//le uuid du service
#define SERVICE_UUID        "19B10000-E8F2-537E-4F6C-D104768A1214"
//le uuid de la characteristic de la temperature
#define TEMP_CHARACTERISTIC_UUID "19B10001-E8F2-537E-4F6C-D104768A1214"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool isDeviceConnected;

//callabck de connection et de déconnexion pour le server ble
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      BLEDevice::startAdvertising();
      isDeviceConnected = true;
      Serial.println("cooonneected");
    };

    void onDisconnect(BLEServer* pServer) {
       isDeviceConnected = false;
    }
};

BleEsp::BleEsp() {

}

void BleEsp::initBle() {
    isDeviceConnected = false;
  // Create the BLE Device
    BLEDevice::init("ESP32");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic en mode read write notify et indicate
    pCharacteristic = pService->createCharacteristic(
                            TEMP_CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ   |
                            BLECharacteristic::PROPERTY_WRITE  |
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_INDICATE
                            );


    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    //pAdvertising->setScanResponse(false);
    //pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

String BleEsp::getTempValue() {
    if(isDeviceConnected) {
        Serial.println("return temp value");
        return pCharacteristic->getValue().c_str();
    } else {
           Serial.println("temp value device not connected");
        return "";
    }
   
}

void BleEsp::setIsDeviceConnected(bool value) {
    isDeviceConnected = value;
    Serial.println("set device connected to true");
    Serial.println(isDeviceConnected);
}

bool BleEsp::getIsDeviceConnected() {
    return isDeviceConnected;
}