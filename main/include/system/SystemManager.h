#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <vector>
#include "interfaces/Sensor.h"
#include "interfaces/Actuator.h"
#include "DataHub.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "communication/NetworkManager.h"

class LogicEngine; // Forward declaration

class SystemManager {
private:
    std::vector<Sensor*> sensors;
    std::vector<Actuator*> actuators;
    
    DataHub* dataHub;
    LogicEngine* logicEngine;
    
public:
    NetworkManager netManager;

private:
    // Task Handles
    TaskHandle_t sensingTaskHandle;
    TaskHandle_t logicTaskHandle;
    TaskHandle_t commsTaskHandle;
    TaskHandle_t safetyTaskHandle;

public:
    SystemManager();
    void registerSensor(Sensor* sensor);
    void registerActuator(Actuator* actuator);
    void init();
    void startTasks();

    // Task Functions (Static wrappers to run in FreeRTOS)
    static void sensingTask(void* pvParameters);
    static void logicTask(void* pvParameters);
    static void commsTask(void* pvParameters);
    static void safetyTask(void* pvParameters);

    // Getters
    std::vector<Sensor*>& getSensors();
    Sensor* getSensor(const char* name);
    Actuator* getActuator(const char* name);
    DataHub* getDataHub() { return dataHub; }
};

#endif // SYSTEM_MANAGER_H
