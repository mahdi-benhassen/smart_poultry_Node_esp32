#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "interfaces/Sensor.h"
#include <DHT.h>
#include "Config.h"

class DHTSensor : public Sensor {
private:
    DHT dht;
    float temperature;
    float humidity;

public:
    DHTSensor(uint8_t pin, uint8_t type);
    void init() override;
    void read() override;
    String report() override;
    const char* getName() override;
    float getTemperature();
    float getHumidity();
};

#endif // DHT_SENSOR_H
