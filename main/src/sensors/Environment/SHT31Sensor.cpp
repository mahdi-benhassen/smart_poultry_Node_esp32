#include "sensors/Environment/SHT31Sensor.h"
#include "esp_log.h"
#include <sstream>

static const char *TAG = "SHT31Sensor";

SHT31Sensor::SHT31Sensor(uint8_t i2cAddress) : address(i2cAddress), lastTemp(0.0), lastHum(0.0) {}

void SHT31Sensor::init() {
    ESP_LOGI(TAG, "Initialized SHT31 at 0x%02x", address);
}

void SHT31Sensor::read() {
    lastTemp = 24.0;
    lastHum = 55.0;
}

std::string SHT31Sensor::report() {
    std::stringstream ss;
    ss << "{\"temp\":" << lastTemp << ",\"hum\":" << lastHum << "}";
    return ss.str();
}

const char* SHT31Sensor::getName() {
    return "SHT31";
}
