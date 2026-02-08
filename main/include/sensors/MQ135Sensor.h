#ifndef MQ135_SENSOR_H
#define MQ135_SENSOR_H

#include "interfaces/Sensor.h"
#include <MQ135.h>
#include "Config.h"

class MQ135Sensor : public Sensor {
private:
    MQ135 mq135;
    float ppm;

public:
    MQ135Sensor(uint8_t pin);
    void init() override;
    void read() override;
    String report() override;
    const char* getName() override;
    float getPPM();
};

#endif // MQ135_SENSOR_H
