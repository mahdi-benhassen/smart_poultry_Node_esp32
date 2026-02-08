#include "sensors/Security/PIRSensor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <sstream>

static const char *TAG = "PIRSensor";

PIRSensor::PIRSensor(int gpioPin) : pin(gpioPin), motionDetected(false) {}

void PIRSensor::init() {
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
    ESP_LOGI(TAG, "Initialized PIR on Pin %d", pin);
}

void PIRSensor::read() {
    motionDetected = gpio_get_level((gpio_num_t)pin);
}

std::string PIRSensor::report() {
    std::stringstream ss;
    ss << "{\"motion\":" << (motionDetected ? "true" : "false") << "}";
    return ss.str();
}

const char* PIRSensor::getName() {
    return "PIR";
}
