#include <Arduino.h>

#include "wifiEsp.h"
#include "bleEsp.h"
#include <Firebase_ESP_Client.h>
#include "SPIFFS.h"

#include <Preferences.h>

#include <ArduinoJson.h>

//define firebase configuration
#define API_KEY "AIzaSyBqVaYFgabWZ7xAbNdvbZelYMomDBrYTvk"

#define FIREBASE_PROJECT_ID "gobelins-onirama"

//chemin où sera enregisrer les données
String tempDocumentPath = "user/8FawTyOj5LMJ7fy4sUJiWOAW8cG3/device/temperatureData";

WifiEsp wifiEsp;
BleEsp bleEsp;

Preferences preferences;

int BUTTON;
int LED;
int ledflag;

bool isFirebaseInit;
bool isUploading;

const char* userEmail;
const char* userPassword;

bool isBtnPressed;

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

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  userEmail = doc["userEmail"];
  userPassword = doc["userPassword"];
  wifiEsp.setUserAndPassword(userEmail, userPassword);

  // Real world application would store these values in some variables for
  // later use.

  return true;
}

void setup()
{
  //instanciation serial avec un baud rate de 115200
  Serial.begin(115200);

  BUTTON = 5;
  LED = 4;
  ledflag = 0;
  isFirebaseInit = false;

  pinMode(BUTTON,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  Serial.println(esp_get_free_heap_size());
  //-------RECUPERATION FICHIER CONFIG----------
   if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {

    preferences.begin("datas", false);

    Serial.println(preferences.getInt("dataCount", 0));

    if (preferences.getBool("isBtnPressed", false)) {         
      digitalWrite(LED,HIGH);
    }  

      bleEsp.initBle();
      while(!bleEsp.getIsDeviceConnected()) {
        Serial.print(".");
        delay(500);
      }
    Serial.println(esp_get_free_heap_size());
    wifiEsp.initWifi();
    Serial.println(esp_get_free_heap_size());
  }
 
}


void saveDataToFirebase() {
   if(wifiEsp.deleteDoc(FIREBASE_PROJECT_ID, tempDocumentPath)) {
           Serial.println("Doc Deleted");
         } else {
           Serial.println("error doc deleted");
         }
                  
          String content;
          //tempValues.push_back(tempValue);

          FirebaseJson js;
          FirebaseJsonArray arr;
          /*for(int i = 0; i < tempValues.size(); i++) {
            arr.set("[" + String(i) + "]" + "/stringValue", tempValues[i]);
          } */
        int dataToGet = preferences.getInt("dataCount", 0);

        for(int i = 0; i <= dataToGet; i++) {
            String indexStr = "tempData" + String(i);
          
            // Print the counter to Serial Monitor
            String tempStr = preferences.getString(indexStr.c_str(), "");
            Serial.println(tempStr);
            if(tempStr != "") {
              arr.set("[" + String(i) + "]" + "/stringValue", tempStr);
            }
          
        }
        int dataCount =  dataToGet + 1;
        preferences.putInt("dataCount", dataCount);    

        js.set("fields/temperature/arrayValue/values", arr);

        js.toString(content);

        Serial.println(content);
        Serial.print("Create a document... ");

        wifiEsp.createDoc(FIREBASE_PROJECT_ID, tempDocumentPath, content);

}

void loop()
{
  if (digitalRead(BUTTON) == HIGH) {
    if (!preferences.getBool("isBtnPressed", false)) {           
      preferences.putBool("isBtnPressed", true);               
      digitalWrite(LED,HIGH);  
      preferences.putBool("isBtnPressed", true); 
    }                          
    else {                    
      preferences.putBool("isBtnPressed", false);                      
      digitalWrite(LED,LOW);
      preferences.clear();
      ESP.restart();
    } 
  }   

  delay(2000);

  if(preferences.getBool("isBtnPressed", false)) {
    Serial.println(bleEsp.getIsDeviceConnected());

    while (!bleEsp.getIsDeviceConnected())
    {
      Serial.print(".");
      delay(200);
    }
    bleEsp.notifyClient();
    if(bleEsp.getTempValue() != "") {
      int indexCount = preferences.getInt("dataCount", 0);
      String indexStr = "tempData" + String(indexCount);
      Serial.println(indexCount);
      preferences.putString(indexStr.c_str(), bleEsp.getTempValue());
      //tempValues.push_back(bleEsp.getTempValue()); 
      bleEsp.deinitBle();
          
      Serial.println(esp_get_free_heap_size());
      if(!isFirebaseInit) {
        wifiEsp.initFirebase(API_KEY);
        isFirebaseInit = true;
      }
      while(!wifiEsp.getIsFirebaseReady()) {
        Serial.print(".");
        delay(200);
      }
          
      saveDataToFirebase();
      ESP.restart();
    }
  }      

  delay(2000);
}
