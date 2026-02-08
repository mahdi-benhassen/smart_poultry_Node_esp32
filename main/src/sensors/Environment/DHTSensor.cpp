#include "sensors/Environment/DHTSensor.h"
#include "esp_log.h"
#include <sstream>

static const char *TAG = "DHTSensor";

DHTSensor::DHTSensor(int gpioPin, int dhtType) : pin((gpio_num_t)gpioPin), type(dhtType), lastTemp(0.0), lastHum(0.0) {}

void DHTSensor::init() {
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    ESP_LOGI(TAG, "Initialized DHT%d on Pin %d", type, pin);
}

void DHTSensor::read() {
    // Placeholder for actual RMT-based read logic
    // In a real implementation, we would pulse the line and read timing
    // For now, we simulate data to verify architecture
    lastTemp = 25.5 + ((rand() % 10) / 10.0);
    lastHum = 60.0 + ((rand() % 10) / 10.0);
}

std::string DHTSensor::report() {
    std::stringstream ss;
    ss << "{\"temp\":" << lastTemp << ",\"hum\":" << lastHum << "}";
    return ss.str();
}

const char* DHTSensor::getName() {
    return "DHT22";
}
