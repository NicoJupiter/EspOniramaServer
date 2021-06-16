#ifndef BLEESP_H
#define BLEESP_H

#include "Arduino.h"

class BleEsp {

    public : 
        BleEsp();
        void initBle();
        void deinitBle();
        String getTempValue();
        void setIsDeviceConnected(bool value);
        bool getIsDeviceConnected();
        void notifyClient();
};

#endif