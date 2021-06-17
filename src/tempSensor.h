#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Arduino.h"

class TempSensor {
    private:
        int _tempPin;
    public : 
        TempSensor();
        void initTempSensor(int pin);
        int getTempSensorValue();
        
};

#endif