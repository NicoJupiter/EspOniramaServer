#ifndef WIFIESP_H
#define WIFIESP_H

#include "Arduino.h"

class WifiEsp {
    private:
        String _userEmail;
        String _userPassword;
    public : 
        WifiEsp();
        void initWifi();
        void initFirebase(String apiKey);
        bool deleteDoc(String firebaseId, String documentPath);
        bool createDoc(String firebaseId, String documentPath, String content);
        String getDoc(String firebaseId, String documentPath, String mask);
        bool getIsFirebaseReady();
        void setUserAndPassword(String userMail, String password);
};

#endif