#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"
#include "driver/gpio.h"

class DHTSensor : public Sensor {
private:
    gpio_num_t pin;
    int type; // 11 or 22
    float lastTemp;
    float lastHum;

public:
    DHTSensor(int gpioPin, int dhtType);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;

    float getTemperature() { return lastTemp; }
    float getHumidity() { return lastHum; }
};

#endif // DHT_SENSOR_H
