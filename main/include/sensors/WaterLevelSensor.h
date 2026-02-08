#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#include "interfaces/Sensor.h"
#include <Arduino.h>

class WaterLevelSensor : public Sensor {
private:
    uint8_t pin;
    int rawValue;

public:
    WaterLevelSensor(uint8_t pin);
    void init() override;
    void read() override;
    String report() override;
    const char* getName() override;
    bool isLow(); // Logic helper
};

#endif // WATER_LEVEL_SENSOR_H
