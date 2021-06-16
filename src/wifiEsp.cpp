#include "wifiEsp.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

WifiEsp::WifiEsp() {

}

void WifiEsp::initWifi() {
    
    WiFiManager wm;
    bool res;
    //création d'un portail avec comme nom de réseau AutoConnectAP et en mdp password
    res = wm.autoConnect("AutoConnectAP","password");

    if(!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } 
    else {
        Serial.println("connected...yeey :)");
    }
}
/**
  * @brief  Init firebase
  */
void WifiEsp::initFirebase(String apiKey, String userMail, String password) {
    
    config.api_key = apiKey.c_str();

    auth.user.email =  "";
    auth.user.password = "";

    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    
    Firebase.reconnectWiFi(true);
}

/**
  * @brief  delete doc
  * note : ne marche que lorsque firebase est ready
  * @return bool
  */
bool WifiEsp::deleteDoc(String firebaseId, String documentPath) {
    if (Firebase.Firestore.deleteDocument(&fbdo, firebaseId.c_str(), "" , documentPath.c_str())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return true;
        } else {
        Serial.println(fbdo.errorReason());
        return false;
    }
  
}
/**
  * @brief  create doc
  * note : ne marche que lorsque firebase est ready
  * @return bool
  */
bool WifiEsp::createDoc(String firebaseId, String documentPath, String content) {
    if (Firebase.Firestore.createDocument(&fbdo, firebaseId.c_str(), "" , documentPath.c_str(), content.c_str())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return true;
    } else {
        Serial.println(fbdo.errorReason());
        return false;
    }
  
}

bool WifiEsp::getIsFirebaseReady() {
    return Firebase.ready();
}