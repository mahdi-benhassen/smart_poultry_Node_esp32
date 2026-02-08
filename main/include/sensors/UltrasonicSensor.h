#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "interfaces/Sensor.h"
#include <Arduino.h>

class UltrasonicSensor : public Sensor {
private:
    uint8_t trigPin;
    uint8_t echoPin;
    float distanceCm;

public:
    UltrasonicSensor(uint8_t trig, uint8_t echo);
    void init() override;
    void read() override;
    String report() override;
    const char* getName() override;
    float getDistance(); // Helper for logic
};

#endif // ULTRASONIC_SENSOR_H
