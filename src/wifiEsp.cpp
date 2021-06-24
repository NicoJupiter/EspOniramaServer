#include "wifiEsp.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

#define WIFI_SSID "Oui bonsoir"
#define WIFI_PASSWORD "OuiOuiNon24"

WifiEsp::WifiEsp() {

}

void WifiEsp::initWifi() {

   /* WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println("connected...yeey :)");*/
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

String WifiEsp::getSSID() {
    return WiFi.SSID();
}

String WifiEsp::getPassword() {
    return WiFi.psk();
}

int WifiEsp::getWifiStatus() {
    return WiFi.status();
}

/**
  * @brief  Init firebase
  */
void WifiEsp::initFirebase(String apiKey) {
    
    config.api_key = apiKey.c_str();

    auth.user.email =  _userEmail.c_str();
    auth.user.password = _userPassword.c_str();

    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    
    Firebase.reconnectWiFi(true);
}

void WifiEsp::setUserAndPassword(String userMail, String password) {
    _userEmail = userMail;
    _userPassword = password;
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
        //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return true;
    } else {
        Serial.println(fbdo.errorReason());
        return false;
    }
  
}

String WifiEsp::getDoc(String firebaseId, String documentPath, String mask) {
    if (Firebase.Firestore.getDocument(&fbdo, firebaseId.c_str(), "", documentPath.c_str(), mask.c_str())) {
    return fbdo.payload().c_str();
    } else {
       return "nothing found";
    }
}

bool WifiEsp::getIsFirebaseReady() {
    return Firebase.ready();
}