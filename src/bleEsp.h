#ifndef BLEESP_H
#define BLEESP_H

#include "Arduino.h"

class BleEsp {

    public : 
        BleEsp();
        void initBle();
        String getTempValue();
        void setIsDeviceConnected(bool value);
        bool getIsDeviceConnected();
};

#endif