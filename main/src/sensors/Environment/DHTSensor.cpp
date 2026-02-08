#include "sensors/Environment/DHTSensor.h"
#include "esp_log.h"
#include <sstream>
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h" // Added for vTaskDelay
#include "freertos/task.h"     // Added for vTaskDelay

static const char *TAG = "DHTSensor";

DHTSensor::DHTSensor(int gpioPin, int dhtType) : pin((gpio_num_t)gpioPin), type(dhtType), lastTemp(0.0), lastHum(0.0) {}

void DHTSensor::init() {
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    ESP_LOGI(TAG, "Initialized DHT%d on Pin %d", type, pin);
    
    // Wait for sensor to stabilize
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// Helper to wait for pin state
static int wait_for_level(gpio_num_t pin, int level, int timeout_us) {
    int elapsed = 0;
    while (gpio_get_level(pin) != level) {
        if (elapsed > timeout_us) return -1;
        ets_delay_us(1);
        elapsed++;
    }
    return elapsed;
}

void DHTSensor::read() {
    uint8_t data[5] = {0};
    
    // 1. Start Signal
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(18000); // Pull low for at least 18ms
    gpio_set_level(pin, 1);
    ets_delay_us(40);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // 2. Wait for Response
    if (wait_for_level(pin, 0, 80) == -1) { ESP_LOGW(TAG, "Timeout waiting for start low"); return; }
    if (wait_for_level(pin, 1, 80) == -1) { ESP_LOGW(TAG, "Timeout waiting for start high"); return; }
    if (wait_for_level(pin, 0, 80) == -1) { ESP_LOGW(TAG, "Timeout waiting for data start"); return; }

    // 3. Read Data (40 bits)
    for (int i = 0; i < 40; i++) {
        if (wait_for_level(pin, 1, 50) == -1) { ESP_LOGW(TAG, "Timeout waiting for bit start"); return; }
        
        // Measure duration of high pulse
        int duration = wait_for_level(pin, 0, 70);
        if (duration == -1) { ESP_LOGW(TAG, "Timeout waiting for bit end"); return; }
        
        // If high pulse > 28us, it's a '1', else '0'
        // Typical '0' is 26-28us, '1' is 70us. Threshold at 40us.
        if (duration > 40) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    // 4. Checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        float h = (data[0] << 8 | data[1]) / 10.0;
        float t = (data[2] << 8 | data[3]) / 10.0;
        
        // Handle negative temperature
        if (data[2] & 0x80) {
            t = -1 * ((data[2] & 0x7F) << 8 | data[3]) / 10.0;
        }

        lastHum = h;
        lastTemp = t;
        // ESP_LOGI(TAG, "Read Success: Temp=%.1f C, Hum=%.1f %%", lastTemp, lastHum);
    } else {
        ESP_LOGW(TAG, "Checksum Error: %02x != %02x", data[4], (data[0] + data[1] + data[2] + data[3]) & 0xFF);
    }
}

std::string DHTSensor::report() {
    std::stringstream ss;
    ss << "{\"temp\":" << lastTemp << ",\"hum\":" << lastHum << "}";
    return ss.str();
}

const char* DHTSensor::getName() {
    return "DHT22";
}
