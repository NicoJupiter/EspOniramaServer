#ifndef WIFIESP_H
#define WIFIESP_H

#include "Arduino.h"

class WifiEsp {
    private:
        String firebaseProjectId;
        String documentPath;
    public : 
        WifiEsp();
        void initWifi(String apiKey, String userMail, String password);
        void initFirebase(String apiKey, String userMail, String password);
        bool deleteDoc(String firebaseId, String documentPath);
        bool createDoc(String firebaseId, String documentPath, String content);
        bool getIsFirebaseReady();
};

#endif