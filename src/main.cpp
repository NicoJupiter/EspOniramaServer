#include <Arduino.h>

#include <Preferences.h>

#include "wifiEsp.h"
#include "bleEsp.h"
#include <Firebase_ESP_Client.h>
#include "SPIFFS.h"

#include <ArduinoJson.h>

//define firebase configuration
#define API_KEY "AIzaSyBqVaYFgabWZ7xAbNdvbZelYMomDBrYTvk"

#define FIREBASE_PROJECT_ID "gobelins-onirama"

//chemin où sera enregisrer les données
String tempDocumentPath = "user/8FawTyOj5LMJ7fy4sUJiWOAW8cG3/device/temperatureData";

WifiEsp wifiEsp;
BleEsp bleEsp;

//va nous permettre de sauvegarder des données dans la mémoire de l'esp32
Preferences preferences;

int indexTemp = 0;
int dataToGet = 5;
int modeIdx;
const int modeAddr = 0;

const int BUTTON = 2;
const int LED = 4;
int BUTTONstate = 0;
bool isDocumentCreated = false;

const char* userEmail;
const char* userPassword;

std::vector<String> tempValues;

//récupération des données contenu dans le fichier config.json
bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  userEmail = doc["userEmail"];
  userPassword = doc["userPassword"];

  // Real world application would store these values in some variables for
  // later use.

  return true;
}

void setup()
{
  //instanciation serial avec un baud rate de 115200
  Serial.begin(115200);

  //-------RECUPERATION FICHIER CONFIG----------
   if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
    //on créer un objet json sous forme {datas : }
    preferences.begin("datas", false);
    Serial.write(preferences.getBool("wifiMode", false));
      //-------GESTION MODE----------
      //check if {datas : {wifiMode: true}}
      if(preferences.getBool("wifiMode", false)) {
        //WIFI MODE
        Serial.println("WIFI MODE");
        wifiEsp.initWifi(API_KEY, userEmail, userPassword);
      } else {
        //BLE MODE
        preferences.clear();
        Serial.println("BLE MODE");
        bleEsp.initBle();
      }

  }

  /*pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);*/

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
   if(wifiEsp.deleteDoc(FIREBASE_PROJECT_ID, tempDocumentPath)) {
           Serial.println("Doc Deleted");
         } else {
           Serial.println("error doc deleted");
         }
                  
          String content;
          //tempValues.push_back(tempValue);

          FirebaseJson js;
          FirebaseJsonArray arr;
                  
          for(int i = 0; i < dataToGet; i++) {
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

        wifiEsp.createDoc(FIREBASE_PROJECT_ID, tempDocumentPath, content);
        isDocumentCreated = true;
        preferences.clear();
        
}

void BleLoop() {
  if(indexTemp < dataToGet) {
    Serial.println("Add value to tempArray");
    
    String indexStr = "tempData" + String(indexTemp);
            
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
