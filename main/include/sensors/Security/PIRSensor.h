#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class PIRSensor : public Sensor {
private:
    int pin;
    bool motionDetected;

public:
    PIRSensor(int gpioPin);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
    SensorType getType() override { return SensorType::PIR; }
    bool isMotionDetected() { return motionDetected; }
};

#endif // PIR_SENSOR_H
