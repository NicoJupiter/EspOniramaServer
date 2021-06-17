#include "tempSensor.h"

int tempVal;  
float volts; 
float temp; 

TempSensor::TempSensor() {

}

void TempSensor::initTempSensor(int pin) {
    _tempPin = pin;
}

int TempSensor::getTempSensorValue() {
    tempVal = analogRead(_tempPin);
    volts = tempVal/1023.0;    
    temp = (volts - 0.5) * 100 ;  
    return temp;
}