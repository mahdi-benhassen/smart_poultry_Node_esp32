#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "Arduino.h"

#include "Config.h"
#include "system/SystemManager.h"
// #include "communication/NetworkManager.h" // Now included via SystemManager

static const char *TAG = "MAIN";

// Arduino-ESP32 linker satisfaction
void setup() {}
void loop() {}

// Fix for missing symbol in some IDF/Arduino versions
extern "C" void esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us) {
    // Placeholder to satisfy linker. 
    // If dynamic frequency scaling is not used, this is safe.
}

extern "C" void app_main(void)
{
    // Initialize Arduino Core
    initArduino();

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Starting Smart Poultry Node (Native ESP-IDF)...");

    // Initialize System
    SystemManager sysManager;
    sysManager.init();

    // Initialize Network (Now managed inside SystemManager)
    // sysManager.netManager.init(); // Removed

    // Start Application Tasks
    sysManager.startTasks();

    // Main Loop - Tasks are now running, so we can just delay or monitor heap
    while (1) {
        // sysManager.pollSensors(); // Moved to SensingTask
        vTaskDelay(pdMS_TO_TICKS(10000));
        ESP_LOGI(TAG, "System Running... Free Heap: %d", (int)esp_get_free_heap_size());
    }
}
