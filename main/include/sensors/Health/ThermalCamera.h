#ifndef THERMAL_CAMERA_H
#define THERMAL_CAMERA_H

#include "interfaces/Sensor.h"
#include "Config.h"

class ThermalCamera : public Sensor {
private:
    uint8_t address;
    float maxTemp;

public:
    ThermalCamera(uint8_t i2cAddress = 0x69);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
};

#endif // THERMAL_CAMERA_H
