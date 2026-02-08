#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <string>

class Sensor {
public:
    virtual ~Sensor() {}
    virtual void init() = 0;
    virtual void read() = 0;
    virtual std::string report() = 0; // Returns JSON string or formatted data
    virtual const char* getName() = 0;
};

#endif // SENSOR_INTERFACE_H
