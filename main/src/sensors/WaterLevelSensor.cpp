#include "sensors/WaterLevelSensor.h"

WaterLevelSensor::WaterLevelSensor(uint8_t pin) : pin(pin) {
    rawValue = 0;
}

void WaterLevelSensor::init() {
    pinMode(pin, INPUT);
}

void WaterLevelSensor::read() {
    rawValue = analogRead(pin);
}

String WaterLevelSensor::report() {
    StaticJsonDocument<200> doc;
    doc["sensor"] = "WaterLevel";
    doc["raw"] = rawValue;
    doc["status"] = (rawValue < 1000) ? "LOW" : "OK"; // Assuming low resistance/voltage = water present or vice versa. 
                                                     // Adjust based on sensor type.
    String output;
    serializeJson(doc, output);
    return output;
}

const char* WaterLevelSensor::getName() {
    return "WaterLevel";
}

bool WaterLevelSensor::isLow() {
    return rawValue < 1000; // Example threshold
}
