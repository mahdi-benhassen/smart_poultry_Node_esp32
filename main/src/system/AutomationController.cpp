#include "system/AutomationController.h"
#include "system/Logger.h"

// Include specific sensors for casting
#include "sensors/Environment/DHTSensor.h"
#include "sensors/Environment/MQ137Sensor.h"
#include "sensors/Resource/FeedLevelSensor.h"

void AutomationController::run(SystemManager& sysManager) {
    // 1. Climate Control (DHT -> Fan/Heater)
    Sensor* sDHT = sysManager.getSensor("DHT22");
    Actuator* aFan = sysManager.getActuator("Fan");
    Actuator* aHeater = sysManager.getActuator("Heater");

    if (sDHT) {
        // Cast safely if you are sure about the type or use dynamic_cast (RTTI needs to be enabled)
        // Here we rely on name string contract
        DHTSensor* dht = (DHTSensor*)sDHT; 
        float temp = dht->getTemperature();

        // Rule: High Temp -> Fan ON
        if (temp > THRESHOLD_TEMP_HIGH) {
            if (aFan && !aFan->isOn()) {
                aFan->activate();
                Logger::log("AUTO", "High Temp -> Fan ON");
            }
            if (aHeater && aHeater->isOn()) {
                aHeater->deactivate();
                Logger::log("AUTO", "High Temp -> Heater OFF");
            }
        } 
        // Rule: Low Temp -> Heater ON
        else if (temp < THRESHOLD_TEMP_LOW) {
            if (aHeater && !aHeater->isOn()) {
                aHeater->activate();
                Logger::log("AUTO", "Low Temp -> Heater ON");
            }
            if (aFan && aFan->isOn()) {
                aFan->deactivate();
                Logger::log("AUTO", "Low Temp -> Fan OFF");
            }
        }
        // Normal Range
        else {
            if (aFan && aFan->isOn()) aFan->deactivate();
            if (aHeater && aHeater->isOn()) aHeater->deactivate();
        }
    }

    // 2. Air Quality (MQ135/MQ137 -> Fan)
    // Note: Project uses MQ137 for ammonia.
    Sensor* sMQ = sysManager.getSensor("MQ137"); 
    if (sMQ) {
        MQ137Sensor* mq = (MQ137Sensor*)sMQ;
        if (mq->getPPM() > THRESHOLD_AMMONIA_MAX) {
            if (aFan && !aFan->isOn()) {
                aFan->activate();
                Logger::log("AUTO", "High Ammonia -> Fan ON");
            }
        }
    }

    // 3. Feeding (Ultrasonic -> Feeder)
    Sensor* sFeed = sysManager.getSensor("FeedLevel");
    Actuator* aFeeder = sysManager.getActuator("Feeder");
    if (sFeed) {
        FeedLevelSensor* us = (FeedLevelSensor*)sFeed;
        // If distance is large (silo empty), trigger feeder logic? 
        // Or if feed level in trough is low? 
        // Assuming "Feed Level" measures distance from top of silo to feed. 
        // Large distance = Low Feed. 
        if (us->getDistance() > THRESHOLD_FEED_LOW) { // If distance > Threshold (High Distance = Low Level)
            if (aFeeder && !aFeeder->isOn()) {
                aFeeder->activate(); 
                Logger::log("AUTO", "Low Feed -> Feeder ON");
            }
        } else {
             if (aFeeder && aFeeder->isOn()) {
                aFeeder->deactivate();
             }
        }
    }
}
