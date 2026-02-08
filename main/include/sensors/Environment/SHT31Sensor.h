#ifndef SHT31_SENSOR_H
#define SHT31_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class SHT31Sensor : public Sensor {
private:
    uint8_t address;
    float lastTemp;
    float lastHum;

public:
    SHT31Sensor(uint8_t i2cAddress = 0x44);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
};

#endif // SHT31_SENSOR_H
