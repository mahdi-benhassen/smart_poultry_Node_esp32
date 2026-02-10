#ifndef MQ137_SENSOR_H
#define MQ137_SENSOR_H

#include "interfaces/Sensor.h"
#include "Config.h"
#include "esp_adc/adc_oneshot.h"

class MQ137Sensor : public Sensor {
private:
    int pin;
    float ammoniaPPM;
    adc_oneshot_unit_handle_t adcHandle;

public:
    MQ137Sensor(int gpioPin, adc_oneshot_unit_handle_t adcHandle);
    void init() override;
    void read() override;
    std::string report() override;
    const char* getName() override;
    SensorType getType() override { return SensorType::MQ137; }
    float getPPM() { return ammoniaPPM; }
};

#endif // MQ137_SENSOR_H
