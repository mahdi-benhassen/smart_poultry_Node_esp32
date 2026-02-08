#include "sensors/Resource/FeedLevelSensor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <sstream>

static const char *TAG = "FeedLevelSensor";

FeedLevelSensor::FeedLevelSensor(int trig, int echo) : trigPin(trig), echoPin(echo), distanceCm(0.0) {}

void FeedLevelSensor::init() {
    gpio_set_direction((gpio_num_t)trigPin, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)echoPin, GPIO_MODE_INPUT);
    ESP_LOGI(TAG, "Initialized Ultrasonic Trig:%d Echo:%d", trigPin, echoPin);
}

void FeedLevelSensor::read() {
    // Basic blocking implementation for demonstration
    // Trigger
    gpio_set_level((gpio_num_t)trigPin, 0);
    esp_rom_delay_us(2);
    gpio_set_level((gpio_num_t)trigPin, 1);
    esp_rom_delay_us(10);
    gpio_set_level((gpio_num_t)trigPin, 0);

    // Measure echo (Timeout handled roughly)
    // In production, use RMT or interrupts
    int timeout = 20000;
    while (gpio_get_level((gpio_num_t)echoPin) == 0 && timeout-- > 0);
    int64_t start = esp_timer_get_time();
    timeout = 20000;
    while (gpio_get_level((gpio_num_t)echoPin) == 1 && timeout-- > 0);
    int64_t end = esp_timer_get_time();

    if (timeout > 0) {
        float duration = (float)(end - start);
        distanceCm = duration / 58.0;
    } else {
        distanceCm = -1.0; // Error
    }
}

std::string FeedLevelSensor::report() {
    std::stringstream ss;
    ss << "{\"feed_level_cm\":" << distanceCm << "}";
    return ss.str();
}

const char* FeedLevelSensor::getName() {
    return "FeedLevel";
}
