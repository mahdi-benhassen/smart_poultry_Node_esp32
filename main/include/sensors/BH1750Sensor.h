#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include "interfaces/Sensor.h"
#include <BH1750.h>
#include <Wire.h>

class BH1750Sensor : public Sensor {
private:
    BH1750 lightMeter;
    float lux;

public:
    BH1750Sensor();
    void init() override;
    void read() override;
    String report() override;
    const char* getName() override;
};

#endif // BH1750_SENSOR_H
