#ifndef FEED_LEVEL_SENSOR_H
#define FEED_LEVEL_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"

class FeedLevelSensor : public Sensor {
private:
    int trigPin;
    int echoPin;
    float distanceCm;

public:
    FeedLevelSensor(int trig, int echo);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
    float getDistance() { return distanceCm; }
};

#endif // FEED_LEVEL_SENSOR_H
