#include "sensors/Environment/MQ137Sensor.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include <sstream>

static const char *TAG = "MQ137Sensor";
static adc_oneshot_unit_handle_t adc1_handle;

MQ137Sensor::MQ137Sensor(int gpioPin) : pin(gpioPin), ammoniaPPM(0.0) {}

void MQ137Sensor::init() {
    // ADC Init Configuration
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // ADC Channel Configuration
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    // Note: Pin mapping to channel needs to be done based on ESP32 model.
    // Assuming Pin 34 is ADC1_CHANNEL_6 for this example on generic ESP32
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    ESP_LOGI(TAG, "Initialized MQ137 on Pin %d", pin);
}

void MQ137Sensor::read() {
    int raw_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw_value));
    
    // Convert to Voltage (approximate)
    float voltage = raw_value * (3.3 / 4095.0);
    
    // Simple linear approximation for PPM (Needs calibration curve)
    ammoniaPPM = voltage * 10.0; 
}

std::string MQ137Sensor::report() {
    std::stringstream ss;
    ss << "{\"nh3_ppm\":" << ammoniaPPM << "}";
    return ss.str();
}

const char* MQ137Sensor::getName() {
    return "MQ137";
}
