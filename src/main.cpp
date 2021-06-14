#include <Arduino.h>

#include <Preferences.h>

#include "wifiEsp.h"
#include "bleEsp.h"
#include <Firebase_ESP_Client.h>

//define firebase configuration
#define API_KEY "AIzaSyBqVaYFgabWZ7xAbNdvbZelYMomDBrYTvk"

#define FIREBASE_PROJECT_ID "gobelins-onirama"

#define USER_EMAIL ""
#define USER_PASSWORD ""

String documentPath = "user/8FawTyOj5LMJ7fy4sUJiWOAW8cG3/device/temperatureData";

WifiEsp wifiEsp;
BleEsp bleEsp;

#define CONFIG_SW_COEXIST_ENABLE 1

Preferences preferences;

int indexTemp = 0;

int modeIdx;
const int modeAddr = 0;

const int BUTTON = 2;
const int LED = 4;
int BUTTONstate = 0;
bool isDocumentCreated = false;

std::vector<String> tempValues;

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);

  preferences.begin("datas", false);
  if(preferences.getBool("wifiMode", false)) {
    //WIFI MODE
    Serial.println("WIFI MODE");
    wifiEsp.initWifi(API_KEY, USER_EMAIL, USER_PASSWORD);
  } else {
     //BLE MODE
    preferences.clear();
    Serial.println("BLE MODE");
     bleEsp.initBle();
  }

  /*
  ---------count mode-------------
  0 : Btn not pressed
  1 : Ble Mode
  2 : Wifi mode
  */
  /*int countMode = preferences.getInt("countMode", -1);

  countMode++;
  if (preferences.getBool("isButtonPressed", false) && countMode != 0) {
     digitalWrite(LED, HIGH);

    if(preferences.getBool("wifiMode", false) && countMode == 2) {
      //wifi mode
      Serial.println("WIFI MODE");
      wifiEsp.initWifi(API_KEY, USER_EMAIL, USER_PASSWORD);
    } else if(!preferences.getBool("wifiMode", false) && countMode == 1) {
      //BLE mode
      Serial.println("BLE MODE");
      bleEsp.initBle();
    } else {
        preferences.clear();
        ESP.restart();
    }

  } else{
    preferences.clear();
    digitalWrite(LED, LOW);
  }

  preferences.putInt("countMode", countMode);*/
 
}

void WifiLoop() {
   if(wifiEsp.deleteDoc(FIREBASE_PROJECT_ID, documentPath)) {
           Serial.println("Doc Deleted");
         } else {
           Serial.println("error doc deleted");
         }
                  
          String content;
          //tempValues.push_back(tempValue);

          FirebaseJson js;
          FirebaseJsonArray arr;
                  
          for(int i = 0; i < 5; i++) {
            String indexStr = "tempData" + String(i);
          
            // Print the counter to Serial Monitor
            String tempStr = preferences.getString(indexStr.c_str(), "");
            if(tempStr != "") {
              Serial.println(tempStr);
              arr.set("[" + String(i) + "]" + "/stringValue", tempStr);
            } else {
              Serial.println("nothhiiinnnggg");
            }
          }
                  
        js.set("fields/temperature/arrayValue/values", arr);

        js.toString(content);

        Serial.println(content);
        Serial.print("Create a document... ");

        wifiEsp.createDoc(FIREBASE_PROJECT_ID, documentPath, content);
        isDocumentCreated = true;
        preferences.clear();
        
}

void BleLoop() {
  if(indexTemp < 5) {
    Serial.println("Add value to tempArray");
    // Or remove the counter key only
    String indexStr = "tempData" + String(indexTemp);
            
    preferences.remove(indexStr.c_str());

    // Store the counter to the Preferences
    preferences.putString(indexStr.c_str(), bleEsp.getTempValue());
    } else {
      Serial.println("Data is ready");
      preferences.putBool("wifiMode", true);
      ESP.restart();
    }
  indexTemp++;
}

void loop()
{

  if(preferences.getBool("wifiMode", false)) {
    if(wifiEsp.getIsFirebaseReady() && !isDocumentCreated) {
        WifiLoop();
    }
  } else {
    if(bleEsp.getIsDeviceConnected()) {
        BleLoop();
    }
  }

/*  BUTTONstate = digitalRead(BUTTON);
  if (BUTTONstate == HIGH) {
     //start appli en ble
  preferences.putBool("isButtonPressed", true);
  ESP.restart();
  } 

  if(preferences.getBool("isButtonPressed", false)) {
    Serial.println("start data mode");
    if(wifiEsp.getIsFirebaseReady() &&  preferences.getBool("wifiMode", false)) {
      //wifi mode
      WifiLoop();
    } else {
      //BLE mode
      if(bleEsp.getIsDeviceConnected()) {
        BleLoop();
      }
    }
  } else {
    Serial.println("sleep mode");
  }*/

  delay(2000);
}
