#include <Arduino.h>

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

int BUTTON;
int LED;
int ledflag;

bool isFirebaseInit;
bool isUploading;

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
    Serial.println("Config loaded");
    //on créer un objet json sous forme {datas : }

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
          for(int i = 0; i < tempValues.size(); i++) {
            arr.set("[" + String(i) + "]" + "/stringValue", tempValues[i]);
          } 
                  
        js.set("fields/temperature/arrayValue/values", arr);

        js.toString(content);

        Serial.println(content);
        Serial.print("Create a document... ");

        wifiEsp.createDoc(FIREBASE_PROJECT_ID, tempDocumentPath, content);

}
/*
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
}*/

void loop()
{
  if (digitalRead(BUTTON) == HIGH) {
    if (ledflag==0) {           
      ledflag=1;                  
      digitalWrite(LED,HIGH);   
    }                          
    else {                    
      ledflag=0;               
      digitalWrite(LED,LOW);
    } 
  }   

  if(ledflag == 1) {
    if(bleEsp.getIsDeviceConnected()) {
        bleEsp.notifyClient();
        if(bleEsp.getTempValue() != "") {
           tempValues.push_back(bleEsp.getTempValue()); 
          bleEsp.deinitBle();
          
          Serial.println(esp_get_free_heap_size());
          if(!isFirebaseInit) {
            wifiEsp.initFirebase(API_KEY, userEmail, userPassword);
            isFirebaseInit = true;
          }
          while(!wifiEsp.getIsFirebaseReady()) {
            Serial.print(".");
            delay(200);
          }
          
           saveDataToFirebase();
           bleEsp.initBle();
           
        Serial.println(esp_get_free_heap_size());
        }
      }
  }      

  delay(2000);
}
