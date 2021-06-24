#include "nimBleEsp.h"
#include <NimBLEDevice.h>

static NimBLEServer* pServer;

//le uuid du service
#define SERVICE_UUID        "19B10000-E8F2-537E-4F6C-D104768A1214"
//le uuid de la characteristic de la temperature
#define CARDIAC_CHARACTERISTIC_UUID "19B10001-E8F2-537E-4F6C-D104768A1214"
#define NOTIFY_CHARACTERISTIC_UUID "19B10003-E8F2-537E-4F6C-D104768A1214"
NimBLECharacteristic* pNimCardiacCharacteristic = NULL;
NimBLECharacteristic* pNimNotifyCharacteristic = NULL;

class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    void onDisconnect(NimBLEServer* pServer) {
        Serial.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();
    };
    
/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest(){
        Serial.println("Server Passkey Request");
        /** This should return a random 6 digit number for security 
         *  or make your own static passkey as done here.
         */
        return 123456; 
    };

    bool onConfirmPIN(uint32_t pass_key){
        Serial.print("The passkey YES/NO number: ");Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true; 
    };

    void onAuthenticationComplete(ble_gap_conn_desc* desc){
        /** Check that encryption was successful, if not we disconnect the client */  
        if(!desc->sec_state.encrypted) {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            Serial.println("Encrypt connection failed - disconnecting client");
            return;
        }
        Serial.println("Starting BLE work!");
    };
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic){
       
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
       
    };
    /** Called before notification or indication is sent, 
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic) {
        Serial.println("Sending notification to clients");
    };


    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", "; 
        str += NimBLEUtils::returnCodeToString(code);
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if(subValue == 0) {
            str += " Unsubscribed to ";
        }else if(subValue == 1) {
            str += " Subscribed to notfications for ";
        } else if(subValue == 2) {
            str += " Subscribed to indications for ";
        } else if(subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

static CharacteristicCallbacks chrCallbacks;

NimBleEsp::NimBleEsp() {

}

void NimBleEsp::initBle() {
  /** sets device name */
    NimBLEDevice::init("ESP32");

    /** Optional: set the transmit power, default is 3db */
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    
  
    //NimBLEDevice::setSecurityAuth(false, false, true); 
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService* pService = pServer->createService(SERVICE_UUID);
    pNimCardiacCharacteristic = pService->createCharacteristic(
                                               CARDIAC_CHARACTERISTIC_UUID,
                                               NIMBLE_PROPERTY::READ |
                                               NIMBLE_PROPERTY::WRITE |
                                                /** Require a secure connection for read and write access */
                                               NIMBLE_PROPERTY::READ_ENC |  // only allow reading if paired / encrypted
                                               NIMBLE_PROPERTY::WRITE_ENC   // only allow writing if paired / encrypted
                                              );

   pNimNotifyCharacteristic = pService->createCharacteristic(
                                               NOTIFY_CHARACTERISTIC_UUID,
                                               NIMBLE_PROPERTY::READ |
                                               NIMBLE_PROPERTY::WRITE |
                                               NIMBLE_PROPERTY::NOTIFY
                                              );
                                        
    //pTempCharacteristic->setValue("Burger");
    pNimCardiacCharacteristic->setCallbacks(&chrCallbacks);
    pNimNotifyCharacteristic->setCallbacks(&chrCallbacks);
    pService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    /** Add the services to the advertisment data **/
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("Advertising Started");

  

}

void NimBleEsp::notifyClient() {
    pNimNotifyCharacteristic->notify(true);
}

String NimBleEsp::getCardiacValue() {
    return pNimCardiacCharacteristic->getValue().c_str();
}

bool NimBleEsp::isConnectedClient() {
  return pServer->getConnectedCount();
}