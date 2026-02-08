#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <vector>
#include "interfaces/Sensor.h"
#include "interfaces/Actuator.h"

class SystemManager {
private:
    std::vector<Sensor*> sensors;
    std::vector<Actuator*> actuators;

public:
    SystemManager();
    void registerSensor(Sensor* sensor);
    void registerActuator(Actuator* actuator);
    void init();
    void pollSensors();
    void runActuators(); // Example logic
    std::vector<Sensor*>& getSensors();
    Sensor* getSensor(const char* name);
    Actuator* getActuator(const char* name);
};

#endif // SYSTEM_MANAGER_H
