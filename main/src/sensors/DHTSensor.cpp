#include "sensors/DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type) : dht(pin, type) {
    temperature = 0.0;
    humidity = 0.0;
}

void DHTSensor::init() {
    dht.begin();
}

void DHTSensor::read() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t)) {
        temperature = t;
    }
    if (!isnan(h)) {
        humidity = h;
    }
}

String DHTSensor::report() {
    StaticJsonDocument<200> doc;
    doc["sensor"] = "DHT";
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    String output;
    serializeJson(doc, output);
    return output;
}

const char* DHTSensor::getName() {
    return "DHT22";
}

float DHTSensor::getTemperature() {
    return temperature;
}

float DHTSensor::getHumidity() {
    return humidity;
}
