#include <Arduino.h>
#include "Config.h"
#include "system/Logger.h"
#include "system/SystemManager.h"
#include "system/AutomationController.h"
#include "security/SecurityManager.h"
#include "communication/NetworkManager.h"
#include "communication/LoRaManager.h"

// Include Sensor Implementations
#ifdef ENABLE_DHT_SENSOR
#include "sensors/DHTSensor.h"
#endif
#ifdef ENABLE_MQ135_SENSOR
#include "sensors/MQ135Sensor.h"
#endif
#ifdef ENABLE_BH1750_SENSOR
#include "sensors/BH1750Sensor.h"
#endif
#ifdef ENABLE_ULTRASONIC_SENSOR
#include "sensors/UltrasonicSensor.h"
#endif
#ifdef ENABLE_WATER_LEVEL
#include "sensors/WaterLevelSensor.h"
#endif

// Include Actuator Implementations
#ifdef ENABLE_RELAY_FAN
#include "actuators/RelayActuator.h"
#endif

// Global Objects
SystemManager sysManager;
NetworkManager netManager;

// Task Handles
TaskHandle_t TaskSensorHandle;
TaskHandle_t TaskNetworkHandle;
TaskHandle_t TaskLoRaHandle;

// Watchdog Timer
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 10 // 10 seconds

void TaskSensorPoll(void *pvParameters) {
    esp_task_wdt_add(NULL); // Add this task to WDT
    for (;;) {
        sysManager.pollSensors();
        AutomationController::run(sysManager);
        
        // Example: Publish data from sensors
        // This logic could be moved inside pollSensors or a separate reporter
        std::vector<Sensor*>& sensors = sysManager.getSensors();
        for (auto sensor : sensors) {
            String data = sensor->report();
            // Encrypt payload
            String encrypted = SecurityManager::encrypt(data);
            
            if (netManager.isConnected()) {
                String topic = "poultry/sensor/" + String(sensor->getName());
                netManager.publish(topic.c_str(), encrypted.c_str());
            }

            // Also send via LoRa
            #ifdef ENABLE_LORA
            LoRaManager::sendPacket(data);
            #endif
        }

        vTaskDelay(SENSOR_POLL_INTERVAL_MS / portTICK_PERIOD_MS);
        esp_task_wdt_reset(); // Feed watchdog
    }
}

void TaskNetwork(void *pvParameters) {
    esp_task_wdt_add(NULL); // Add this task to WDT
    netManager.setupWiFi();
    netManager.setupOTA();
    // netManager.setServer(MQTT_SERVER, MQTT_PORT); // Removed: Handled in setupWiFi via Preferences
    
    for (;;) {
        netManager.loop();
        esp_task_wdt_reset();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Small delay to yield
    }
}

void TaskLoRa(void *pvParameters) {
    #ifdef ENABLE_LORA
    esp_task_wdt_add(NULL);
    LoRaManager::init();
    for (;;) {
        int packetSize = LoRaManager::parsePacket(); // Thread-safe wrapper
        if (packetSize) {
            LoRaManager::receivePacket(packetSize);
        }
        esp_task_wdt_reset();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    #else
    vTaskDelete(NULL);
    #endif
}

void setup() {
    Logger::init(SERIAL_BAUD_RATE);
    
    // Init Security
    SecurityManager::init();

    // Register Sensors
    #ifdef ENABLE_DHT_SENSOR
        sysManager.registerSensor(new DHTSensor(PIN_DHT, DHT22));
    #endif

    #ifdef ENABLE_MQ135_SENSOR
        sysManager.registerSensor(new MQ135Sensor(PIN_MQ135));
    #endif

    #ifdef ENABLE_BH1750_SENSOR
        sysManager.registerSensor(new BH1750Sensor());
    #endif

    #ifdef ENABLE_ULTRASONIC_SENSOR
        sysManager.registerSensor(new UltrasonicSensor(PIN_ULTRASONIC_TRIG, PIN_ULTRASONIC_ECHO));
    #endif

    #ifdef ENABLE_WATER_LEVEL
        sysManager.registerSensor(new WaterLevelSensor(PIN_WATER_LEVEL));
    #endif

    // Register Actuators
    #ifdef ENABLE_RELAY_FAN
        sysManager.registerActuator(new RelayActuator(PIN_RELAY_FAN, "Fan"));
    #endif
    #ifdef ENABLE_RELAY_HEATER
        sysManager.registerActuator(new RelayActuator(PIN_RELAY_HEATER, "Heater"));
    #endif
    #ifdef ENABLE_RELAY_FEEDER
        sysManager.registerActuator(new RelayActuator(PIN_RELAY_FEEDER, "Feeder"));
    #endif

    sysManager.init();

    // Init Watchdog
    // IDF 5.x / Arduino 3.0 Compatibility
    #if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 5
        esp_task_wdt_config_t twdt_config = {
            .timeout_ms = WDT_TIMEOUT * 1000,
            .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,    // Bitmask of all cores
            .trigger_panic = true,
        };
        esp_task_wdt_init(&twdt_config);
    #else
        esp_task_wdt_init(WDT_TIMEOUT, true);
    #endif
    
    esp_task_wdt_add(NULL); // Add setup/loop task (though loop is deleted)

    // Create Tasks
    xTaskCreatePinnedToCore(
        TaskSensorPoll,   /* Task function. */
        "SensorPoll",     /* name of task. */
        10000,            /* Stack size of task */
        NULL,             /* parameter of the task */
        1,                /* priority of the task */
        &TaskSensorHandle, /* Task handle to keep track of created task */
        1);               /* pin task to core 1 */
        
    xTaskCreatePinnedToCore(
        TaskNetwork,
        "Network",
        10000,
        NULL,
        1,
        &TaskNetworkHandle,
        0); /* pin network task to core 0 */

    xTaskCreatePinnedToCore(
        TaskLoRa,
        "LoRa",
        10000,
        NULL,
        1,
        &TaskLoRaHandle,
        0);
        
    // Register tasks to WDT (Handled inside tasks now)
}

void loop() {
    // Main loop is empty because we use FreeRTOS tasks
    vTaskDelete(NULL);
}
