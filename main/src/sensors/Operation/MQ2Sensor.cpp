#include "sensors/Operation/MQ2Sensor.h"
#include "esp_log.h"
#include <sstream>

static const char *TAG = "MQ2Sensor";

MQ2Sensor::MQ2Sensor(int gpioPin, adc_oneshot_unit_handle_t handle) : pin(gpioPin), smokeValue(0.0), adcHandle(handle) {}

void MQ2Sensor::init() {
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    // Mapping PIN_MQ2 (35) to ADC channel. GPIO35 is ADC1_CHANNEL_7
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle, ADC_CHANNEL_7, &config));

    ESP_LOGI(TAG, "Initialized MQ2 on Pin %d", pin);
}

void MQ2Sensor::read() {
    int raw_value = 0;
    // Read ADC1 Channel 7 (GPIO 35)
    ESP_ERROR_CHECK(adc_oneshot_read(adcHandle, ADC_CHANNEL_7, &raw_value));
    smokeValue = raw_value * (3.3 / 4095.0) * 100.0; // Dummy conversion
}

std::string MQ2Sensor::report() {
    std::stringstream ss;
    ss << "{\"smoke_level\":" << smokeValue << "}";
    return ss.str();
}

const char* MQ2Sensor::getName() {
    return "MQ2";
}
