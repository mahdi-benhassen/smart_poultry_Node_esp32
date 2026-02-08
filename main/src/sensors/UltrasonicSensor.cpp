#include "sensors/UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(uint8_t trig, uint8_t echo) : trigPin(trig), echoPin(echo) {
    distanceCm = 0.0;
}

void UltrasonicSensor::init() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void UltrasonicSensor::read() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms (~5m)
    if (duration == 0) {
        distanceCm = -1.0; // Error
    } else {
        distanceCm = duration * 0.034 / 2;
    }
}

String UltrasonicSensor::report() {
    StaticJsonDocument<200> doc;
    doc["sensor"] = "Ultrasonic";
    doc["distance_cm"] = distanceCm;
    String output;
    serializeJson(doc, output);
    return output;
}

const char* UltrasonicSensor::getName() {
    return "FeedLevel";
}

float UltrasonicSensor::getDistance() {
    return distanceCm;
}
