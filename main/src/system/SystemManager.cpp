#include "system/SystemManager.h"
#include "system/Logger.h"

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
        String data = sensor->report();
        Logger::logf("SYS", "Sensor [%s]: %s", sensor->getName(), data.c_str());
        // Here you would typically send 'data' to MQTT or LoRa
    }
}

void SystemManager::runActuators() {
    // Example logic: Simple thermostat or rule-based control could go here
    // or be delegated to a separate Controller class.
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
