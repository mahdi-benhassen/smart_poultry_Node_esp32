#include "actuators/RelayActuator.h"

RelayActuator::RelayActuator(uint8_t pin, const char* name) : pin(pin), name(name) {
    state = false;
}

void RelayActuator::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Default off
}

void RelayActuator::activate() {
    digitalWrite(pin, HIGH);
    state = true;
}

void RelayActuator::deactivate() {
    digitalWrite(pin, LOW);
    state = false;
}

bool RelayActuator::isOn() {
    return state;
}

const char* RelayActuator::getName() {
    return name;
}
