#include "system/SystemManager.h"
#include "esp_log.h"
#include <cstring>

static const char *TAG = "SystemManager";

SystemManager::SystemManager() {}

void SystemManager::registerSensor(Sensor* sensor) {
    sensors.push_back(sensor);
}

void SystemManager::registerActuator(Actuator* actuator) {
    actuators.push_back(actuator);
}

void SystemManager::init() {
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
