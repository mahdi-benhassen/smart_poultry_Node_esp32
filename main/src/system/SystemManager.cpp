#include "system/SystemManager.h"
#include "system/LogicEngine.h" // Include logic engine implementation
#include "esp_log.h"
#include <cstring>
#include "Config.h"

// Include Sensor Implementations
#include "sensors/Environment/DHTSensor.h"
#include "sensors/Environment/MQ137Sensor.h"
#include "sensors/Environment/BH1750Sensor.h"
#include "sensors/Environment/SHT31Sensor.h"
#include "sensors/Resource/FeedLevelSensor.h"
#include "sensors/Security/PIRSensor.h"
#include "sensors/Health/ThermalCamera.h"
#include "sensors/Operation/MQ2Sensor.h"

// Include Actuator Implementations if available
#include "actuators/RelayActuator.h"

static const char *TAG = "SystemManager";

SystemManager::SystemManager() {
    dataHub = new DataHub();
    logicEngine = new LogicEngine(dataHub, this);
}

void SystemManager::registerSensor(Sensor* sensor) {
    sensors.push_back(sensor);
}

void SystemManager::registerActuator(Actuator* actuator) {
    actuators.push_back(actuator);
}

void SystemManager::init() {
    ESP_LOGI(TAG, "Registering Sensors...");

    #if ENABLE_DHT22
        registerSensor(new DHTSensor(PIN_DHT, 22));
    #endif

    #if ENABLE_MQ137
        registerSensor(new MQ137Sensor(PIN_MQ137));
    #endif

    #if ENABLE_BH1750
        registerSensor(new BH1750Sensor());
    #endif

    #if ENABLE_SHT31
        registerSensor(new SHT31Sensor());
    #endif

    #if ENABLE_HC_SR04
        registerSensor(new FeedLevelSensor(PIN_TRIG, PIN_ECHO));
    #endif

    #if ENABLE_PIR
        registerSensor(new PIRSensor(PIN_PIR));
    #endif
    
    #if ENABLE_AMG8833
        registerSensor(new ThermalCamera());
    #endif

    #if ENABLE_MQ2
        registerSensor(new MQ2Sensor(PIN_MQ2));
    #endif

    // Register Actuators
    #ifdef PIN_RELAY_FAN
    registerActuator(new RelayActuator(PIN_RELAY_FAN, "Fan"));
    #endif
    #ifdef PIN_RELAY_HEATER
    registerActuator(new RelayActuator(PIN_RELAY_HEATER, "Heater"));
    #endif
    #if ENABLE_RELAY_SIREN
    registerActuator(new RelayActuator(PIN_RELAY_SIREN, "Siren"));
    #endif

    ESP_LOGI(TAG, "Initializing Sensors...");
    for (auto sensor : sensors) {
        sensor->init();
    }
    for (auto actuator : actuators) {
        actuator->init();
    }

    // Initialize Network
    ESP_LOGI(TAG, "Initializing Network...");
    netManager->init();
}

void SystemManager::startTasks() {
    ESP_LOGI(TAG, "Starting Application Layer Tasks...");

    // Stack Sizes Optimized
    xTaskCreate(sensingTask, "SensingTask", 3072, this, 5, &sensingTaskHandle);
    xTaskCreate(logicTask,   "LogicTask",   3072, this, 5, &logicTaskHandle);
    xTaskCreate(commsTask,   "CommsTask",   4096, this, 4, &commsTaskHandle); // WiFi/MQTT needs stack
    xTaskCreate(safetyTask,  "SafetyTask",  2048, this, 6, &safetyTaskHandle); // Minimal logic
}

void SystemManager::sensingTask(void* pvParameters) {
    SystemManager* self = (SystemManager*)pvParameters;
    DataHub* hub = self->getDataHub();

    for (;;) {
        // Poll sensors
        for (auto sensor : self->getSensors()) {
            sensor->read();
            
            // Map specific sensors to DataHub
            switch (sensor->getType()) {
                case SensorType::DHT22: {
                    DHTSensor* dht = (DHTSensor*)sensor;
                    hub->setTemperature(dht->getTemperature());
                    // hub->setHumidity(dht->getHumidity());
                    break;
                }
                case SensorType::MQ137: {
                    MQ137Sensor* mq = (MQ137Sensor*)sensor;
                    hub->setAmmonia(mq->getPPM());
                    break;
                }
                case SensorType::MQ2: {
                    MQ2Sensor* mq2 = (MQ2Sensor*)sensor;
                    hub->setSmokeLevel(mq2->getSmokeLevel());
                    break;
                }
                case SensorType::BH1750: {
                    BH1750Sensor* bh = (BH1750Sensor*)sensor;
                    hub->setLux(bh->getLux());
                    break;
                }
                case SensorType::PIR: {
                    PIRSensor* pir = (PIRSensor*)sensor;
                    hub->setMotion(pir->isMotionDetected());
                    break;
                }
                default:
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Poll every 2 seconds
    }
}

void SystemManager::logicTask(void* pvParameters) {
    SystemManager* self = (SystemManager*)pvParameters;
    LogicEngine* engine = self->logicEngine;

    for (;;) {
        engine->process();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Run logic every 1 second
    }
}

void SystemManager::commsTask(void* pvParameters) {
    SystemManager* self = (SystemManager*)pvParameters;
    DataHub* hub = self->getDataHub();
    NetworkManager* net = self->getNetworkManager(); // We need access to netManager

    // Check for OTA update on boot (or periodic)
    // net->checkOTAUpdate();

    for (;;) {
        SystemData snapshot = hub->getSnapshot();
        
        // Simple JSON Construction
        char jsonBuffer[256];
        snprintf(jsonBuffer, sizeof(jsonBuffer), 
            "{\"temp\":%.2f,\"humid\":%.2f,\"smoke\":%.2f,\"motion\":%d,\"state\":%d,\"sec_state\":%d}",
            snapshot.temperature, snapshot.humidity, snapshot.smokeLevel, 
            snapshot.motionDetected, (int)snapshot.currentState, (int)snapshot.securityState);

        net->publish("poultry/status", jsonBuffer);
        
        ESP_LOGI(TAG, "Comms: Broadcasting State %d, SecState %d, Temp %.2f", 
            (int)snapshot.currentState, (int)snapshot.securityState, snapshot.temperature);
        
        // Periodic OTA check (e.g., once a day, or triggered by MQTT command)
        // For now, just a placeholder.
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // Publish every 5 seconds
    }
}

void SystemManager::safetyTask(void* pvParameters) {
    SystemManager* self = (SystemManager*)pvParameters;
    DataHub* hub = self->getDataHub();

    for (;;) {
        // Run Security Logic
        self->securityManager->process();

        // Watchdog Logic
        // If temp > 50C (Fire?), shut down everything immediately
        if (hub->getTemperature() > 50.0) {
            ESP_LOGE(TAG, "SAFETY SHUTDOWN: Temperature Critical!");
            // Hard shutdown actuators
            // self->emergencyStop();
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Check often
    }
}

std::vector<Sensor*>& SystemManager::getSensors() {
    return sensors;
}

Sensor* SystemManager::getSensor(const char* name) {
    for (auto sensor : sensors) {
        if (strcmp(sensor->getName(), name) == 0) {
            return sensor;
        }
    }
    return nullptr;
}

Actuator* SystemManager::getActuator(const char* name) {
    for (auto actuator : actuators) {
        if (strcmp(actuator->getName(), name) == 0) {
            return actuator;
        }
    }
    return nullptr;
}
