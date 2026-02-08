#include "system/LogicEngine.h"
#include "Config.h"

static const char* TAG = "LogicEngine";

LogicEngine::LogicEngine(DataHub* hub, SystemManager* manager) : dataHub(hub), sysManager(manager) {}

void LogicEngine::process() {
    // 1. Fetch Snapshot
    SystemData data = dataHub->getSnapshot();
    
    // 2. State Machine Transition Logic
    HouseState nextState = data.currentState;

    // Emergency Checks (Highest Priority)
    if (data.smokeLevel > 200.0) { // Threshold for fire?
        nextState = HouseState::EMERGENCY_SHUTDOWN;
    }
    else if (data.currentState == HouseState::EMERGENCY_SHUTDOWN) {
        // Require manual reset or very low smoke to exit
        if (data.smokeLevel < 50.0) {
            nextState = HouseState::NORMAL; // Or require manual command
        }
    }
    else {
        // Normal State Logic
        if (data.ammoniaPPM > THRESHOLD_AMMONIA_MAX) {
            nextState = HouseState::GAS_ALARM;
        } else if (data.temperature > THRESHOLD_TEMP_HIGH + 5.0) { // Critical Heat Stress
            nextState = HouseState::HEAT_STRESS;
        } else if (data.lux < THRESHOLD_LUX_MIN) {
            nextState = HouseState::NIGHT_MODE;
        } else {
            // Hysteresis logic
            if (data.currentState == HouseState::GAS_ALARM && data.ammoniaPPM < (THRESHOLD_AMMONIA_MAX - 5.0)) {
                nextState = HouseState::NORMAL;
            } else if (data.currentState == HouseState::HEAT_STRESS && data.temperature < THRESHOLD_TEMP_HIGH) {
                nextState = HouseState::NORMAL;
            } else if (data.currentState == HouseState::NIGHT_MODE && data.lux > THRESHOLD_LUX_MIN + 5.0) {
                nextState = HouseState::NORMAL;
            }
        }
    }
    
    // Update State if changed
    if (nextState != data.currentState) {
        ESP_LOGI(TAG, "State Transition: %d -> %d", (int)data.currentState, (int)nextState);
        dataHub->setState(nextState);
        data.currentState = nextState; // Update local copy
    }

    // 3. Execute State Logic
    switch (data.currentState) {
        case HouseState::NORMAL:
            handleNormalState(data.temperature, data.ammoniaPPM);
            break;
        case HouseState::HEAT_STRESS:
            handleHeatStressState(data.temperature);
            break;
        case HouseState::GAS_ALARM:
            handleGasAlarmState(data.ammoniaPPM);
            break;
        case HouseState::NIGHT_MODE:
            handleNightMode();
            break;
        case HouseState::EMERGENCY_SHUTDOWN:
            // Stop everything, sound alarm
            {
                Actuator* fan = sysManager->getActuator("Fan");
                Actuator* heater = sysManager->getActuator("Heater");
                if (fan) fan->deactivate(); // Stop fanning fire
                if (heater) heater->deactivate();
                ESP_LOGE(TAG, "EMERGENCY SHUTDOWN ACTIVE");
            }
            break;
        default:
            break;
    }
}

void LogicEngine::handleNormalState(float temp, float nh3) {
    // Business Logic for Normal Operation
    // Maintain temp between 20-25C
    Actuator* fan = sysManager->getActuator("Fan");
    Actuator* heater = sysManager->getActuator("Heater");

    if (temp > THRESHOLD_TEMP_HIGH) {
        if (fan && !fan->isManualMode()) fan->activate();
        if (heater && !heater->isManualMode()) heater->deactivate();
    } else if (temp < THRESHOLD_TEMP_LOW) {
        if (fan && !fan->isManualMode()) fan->deactivate();
        if (heater && !heater->isManualMode()) heater->activate();
    } else {
        // Comfort zone
        if (fan && !fan->isManualMode()) fan->deactivate();
        if (heater && !heater->isManualMode()) heater->deactivate();
    }
}

void LogicEngine::handleHeatStressState(float temp) {
    ESP_LOGW(TAG, "CRITICAL: Heat Stress! Temp: %.2f", temp);
    // Maximize Ventilation, Turn off Heaters
    Actuator* fan = sysManager->getActuator("Fan");
    Actuator* heater = sysManager->getActuator("Heater");
    
    // Safety override: Force fans ON even in manual mode? 
    // Usually safety overrides manual, but for now let's respect manual unless it's a safety task.
    if (fan && !fan->isManualMode()) fan->activate(); 
    if (heater && !heater->isManualMode()) heater->deactivate();
}

void LogicEngine::handleGasAlarmState(float nh3) {
    ESP_LOGE(TAG, "CRITICAL: Ammonia Gas Alarm! PPM: %.2f", nh3);
    // Max Ventilation to clear gas
    Actuator* fan = sysManager->getActuator("Fan");
    if (fan && !fan->isManualMode()) fan->activate();
}

void LogicEngine::handleNightMode() {
    // Quiet operation, ensure lights are off if we control them
    // Maintain lower temp setpoint if needed?
    // For now, just logging
    ESP_LOGI(TAG, "Night Mode Active");
}
