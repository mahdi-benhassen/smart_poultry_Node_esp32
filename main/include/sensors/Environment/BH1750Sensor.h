#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class BH1750Sensor : public Sensor {
private:
    uint8_t address;
    float lux;

public:
    BH1750Sensor(uint8_t i2cAddress = 0x23);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
};

#endif // BH1750_SENSOR_H
