#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <string>

enum class SensorType {
    GENERIC,
    DHT22,
    MQ137,
    MQ2,
    BH1750,
    PIR,
    ULTRASONIC,
    THERMAL_CAMERA,
    SHT31
};

class Sensor {
public:
    virtual ~Sensor() {}
    virtual void init() = 0;
    virtual void read() = 0;
    virtual std::string report() = 0; // Returns JSON string or formatted data
    virtual const char* getName() = 0;
    virtual SensorType getType() { return SensorType::GENERIC; }
};

#endif // SENSOR_INTERFACE_H
