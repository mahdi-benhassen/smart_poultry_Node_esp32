#include "sensors/MQ135Sensor.h"

MQ135Sensor::MQ135Sensor(uint8_t pin) : mq135(pin) {
    ppm = 0.0;
}

void MQ135Sensor::init() {
    // Calibration logic can go here or be skipped for now
}

void MQ135Sensor::read() {
    ppm = mq135.getPPM();
}

String MQ135Sensor::report() {
    StaticJsonDocument<200> doc;
    doc["sensor"] = "MQ135";
    doc["ammonia_ppm"] = ppm;
    String output;
    serializeJson(doc, output);
    return output;
}

const char* MQ135Sensor::getName() {
    return "MQ135";
}

float MQ135Sensor::getPPM() {
    return ppm;
}
