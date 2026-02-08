#ifndef LOGIC_ENGINE_H
#define LOGIC_ENGINE_H

#include "DataHub.h"
#include "SystemManager.h"
#include "esp_log.h"

class LogicEngine {
private:
    DataHub* dataHub;
    SystemManager* sysManager;
    
    // Internal FSM Handlers
    void handleNormalState(float temp, float nh3);
    void handleHeatStressState(float temp);
    void handleGasAlarmState(float nh3);
    void handleNightMode();
    
    // Rule Engine Helpers
    void checkEnvironmentalRules(float temp, float nh3);
    void checkSafetyRules(float temp, float nh3);

public:
    LogicEngine(DataHub* hub, SystemManager* manager);
    void process(); // Called periodically by LogicTask
};

#endif // LOGIC_ENGINE_H
