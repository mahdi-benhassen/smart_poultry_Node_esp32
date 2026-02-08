#ifndef MQ137_SENSOR_H
#define MQ137_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class MQ137Sensor : public Sensor {
private:
    int pin;
    float ammoniaPPM;

public:
    MQ137Sensor(int gpioPin);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
    float getPPM() { return ammoniaPPM; }
};

#endif // MQ137_SENSOR_H
