#include "sensors/Health/ThermalCamera.h"
#include "esp_log.h"
#include <sstream>

static const char *TAG = "ThermalCamera";

ThermalCamera::ThermalCamera(uint8_t i2cAddress) : address(i2cAddress), maxTemp(0.0) {}

void ThermalCamera::init() {
    ESP_LOGI(TAG, "Initialized AMG8833 at 0x%02x", address);
}

void ThermalCamera::read() {
    maxTemp = 38.5; // Dummy fever detection
}

std::string ThermalCamera::report() {
    std::stringstream ss;
    ss << "{\"max_body_temp\":" << maxTemp << "}";
    return ss.str();
}

const char* ThermalCamera::getName() {
    return "ThermalCam";
}
