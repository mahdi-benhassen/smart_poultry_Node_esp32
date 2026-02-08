#include "actuators/RelayActuator.h"
#include "driver/gpio.h"

RelayActuator::RelayActuator(uint8_t pin, const char* name) : pin(pin), name(name) {
    state = false;
    manualMode = false;
}

void RelayActuator::init() {
    gpio_reset_pin((gpio_num_t)pin);
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)pin, 0); // Default off
}

void RelayActuator::activate() {
    gpio_set_level((gpio_num_t)pin, 1);
    state = true;
}

void RelayActuator::deactivate() {
    gpio_set_level((gpio_num_t)pin, 0);
    state = false;
}

bool RelayActuator::isOn() {
    return state;
}

const char* RelayActuator::getName() {
    return name;
}

void RelayActuator::setManualMode(bool manual) {
    manualMode = manual;
}

bool RelayActuator::isManualMode() {
    return manualMode;
}
