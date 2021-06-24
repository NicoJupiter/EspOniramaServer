#ifndef NIMBLEESP_H
#define NIMBLEESP_H

#include "Arduino.h"

class NimBleEsp {

    public : 
        NimBleEsp();
        void initBle();
        bool isConnectedClient();
        String getCardiacValue();
        void notifyClient();
};

#endif