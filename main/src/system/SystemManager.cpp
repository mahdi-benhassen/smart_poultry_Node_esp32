#include "system/SystemManager.h"
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

static const char *TAG = "SystemManager";

SystemManager::SystemManager() {}

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

    ESP_LOGI(TAG, "Initializing Sensors...");
    for (auto sensor : sensors) {
        sensor->init();
    }
    for (auto actuator : actuators) {
        actuator->init();
    }
}

void SystemManager::pollSensors() {
    for (auto sensor : sensors) {
        sensor->read();
        std::string data = sensor->report();
        ESP_LOGI(TAG, "Sensor [%s]: %s", sensor->getName(), data.c_str());
    }
}

void SystemManager::runActuators() {
    // Logic placeholder
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
