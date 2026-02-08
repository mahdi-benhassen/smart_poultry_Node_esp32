#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class MQ2Sensor : public Sensor {
private:
    int pin;
    float smokeValue;

public:
    MQ2Sensor(int gpioPin);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
    SensorType getType() override { return SensorType::MQ2; }
    float getSmokeLevel() { return smokeValue; }
};

#endif // MQ2_SENSOR_H
