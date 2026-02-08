#include "system/AutomationController.h"
#include <Arduino.h>
#include "system/Logger.h"

// Include specific sensors for casting
#include "sensors/DHTSensor.h"
#include "sensors/MQ135Sensor.h"
#include "sensors/UltrasonicSensor.h"
#include "sensors/WaterLevelSensor.h"

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

    // 2. Air Quality (MQ135 -> Fan)
    Sensor* sMQ = sysManager.getSensor("MQ135");
    if (sMQ) {
        MQ135Sensor* mq = (MQ135Sensor*)sMQ;
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
        UltrasonicSensor* us = (UltrasonicSensor*)sFeed;
        // If distance is large (silo empty), trigger feeder logic? 
        // Or if feed level in trough is low? 
        // Assuming "Feed Level" measures distance from top of silo to feed. 
        // Large distance = Low Feed. 
        // Wait, typically "Feed Level" in prompt might mean "Amount left".
        // Let's assume Distance > Threshold means EMPTY.
        if (us->getDistance() > THRESHOLD_FEED_LOW) { // If distance > 10cm, maybe that means empty?
            // Actually usually: Low Feed = High Distance. 
            // Let's assume user wants to REFILL if level is low.
            // But "Feeder" usually dispenses to birds.
            // Let's assume: If Feed in Trough (Ultrasonic pointing at trough) is Low (High Distance), Activate Feeder.
            if (aFeeder && !aFeeder->isOn()) {
                aFeeder->activate(); 
                // In real world, we would run it for X seconds then stop. 
                // This simple logic might keep it on until filled.
                Logger::log("AUTO", "Low Feed -> Feeder ON");
            }
        } else {
             if (aFeeder && aFeeder->isOn()) {
                aFeeder->deactivate();
             }
        }
    }
}
