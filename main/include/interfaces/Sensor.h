#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
public:
    virtual ~Sensor() {}
    virtual void init() = 0;
    virtual void read() = 0;
    virtual String report() = 0; // Returns JSON string or formatted data
    virtual const char* getName() = 0;
};

#endif // SENSOR_INTERFACE_H
