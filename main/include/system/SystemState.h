#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <string>
#include <map>

// Finite State Machine States
enum class HouseState {
    NORMAL,
    HEAT_STRESS,
    GAS_ALARM,
    NIGHT_MODE,
    EMERGENCY_SHUTDOWN,
    MAINTENANCE_MODE
};

// Sensor Data Structure (Data Marshalling)
struct SystemData {
    float temperature;
    float humidity;
    float ammoniaPPM;
    float co2PPM;
    float feedLevel;
    float waterLevel;
    float lux;         // Light Intensity
    float smokeLevel;  // Smoke/Gas from MQ2
    bool motionDetected;
    bool fireDetected;
    
    // Metadata
    uint32_t timestamp;
    HouseState currentState;
};

#endif // SYSTEM_STATE_H
