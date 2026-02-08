#include "sensors/BH1750Sensor.h"
#include "Config.h"
#include "system/Logger.h"

BH1750Sensor::BH1750Sensor() {
    lux = 0.0;
}

void BH1750Sensor::init() {
    Wire.begin(PIN_BH1750_SDA, PIN_BH1750_SCL);
    if (lightMeter.begin()) {
        Logger::log("BH1750", "Initialized");
    } else {
        Logger::log("BH1750", "Error initializing");
    }
}

void BH1750Sensor::read() {
    lux = lightMeter.readLightLevel();
}

String BH1750Sensor::report() {
    StaticJsonDocument<200> doc;
    doc["sensor"] = "BH1750";
    doc["lux"] = lux;
    String output;
    serializeJson(doc, output);
    return output;
}

const char* BH1750Sensor::getName() {
    return "BH1750";
}
