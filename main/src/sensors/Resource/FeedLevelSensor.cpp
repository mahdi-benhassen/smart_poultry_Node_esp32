#include "sensors/Resource/FeedLevelSensor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "rom/ets_sys.h" // For ets_delay_us
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
    ets_delay_us(2);
    gpio_set_level((gpio_num_t)trigPin, 1);
    ets_delay_us(10);
    gpio_set_level((gpio_num_t)trigPin, 0);

    // Measure echo (Timeout handled with time check)
    int64_t start_wait = esp_timer_get_time();
    while (gpio_get_level((gpio_num_t)echoPin) == 0) {
        if (esp_timer_get_time() - start_wait > 30000) { // 30ms timeout
             distanceCm = -1.0;
             return;
        }
    }

    int64_t start = esp_timer_get_time();
    while (gpio_get_level((gpio_num_t)echoPin) == 1) {
         if (esp_timer_get_time() - start > 30000) { // 30ms timeout (~5m)
             break;
         }
    }
    int64_t end = esp_timer_get_time();

    float duration = (float)(end - start);
    if (duration > 0 && duration < 30000) {
        distanceCm = duration / 58.0;
    } else {
        distanceCm = -1.0; // Error or Out of range
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
