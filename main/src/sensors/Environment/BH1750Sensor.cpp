#include "sensors/Environment/BH1750Sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include <sstream>

static const char *TAG = "BH1750Sensor";

BH1750Sensor::BH1750Sensor(uint8_t i2cAddress) : address(i2cAddress), lux(0.0) {}

void BH1750Sensor::init() {
    // I2C Master Initialization should ideally be done once in SystemManager
    // Here we assume i2c_param_config and i2c_driver_install have been called
    ESP_LOGI(TAG, "Initialized BH1750 at 0x%02x", address);
}

void BH1750Sensor::read() {
    // Placeholder I2C read
    // i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // i2c_master_start(cmd);
    // ...
    lux = 500.0; // Dummy value
}

std::string BH1750Sensor::report() {
    std::stringstream ss;
    ss << "{\"lux\":" << lux << "}";
    return ss.str();
}

const char* BH1750Sensor::getName() {
    return "BH1750";
}
