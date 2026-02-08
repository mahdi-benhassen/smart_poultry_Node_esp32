#ifndef RELAY_ACTUATOR_H
#define RELAY_ACTUATOR_H

#include "interfaces/Actuator.h"

class RelayActuator : public Actuator {
private:
    uint8_t pin;
    bool state;
    const char* name;

public:
    RelayActuator(uint8_t pin, const char* name);
    void init() override;
    void activate() override;
    void deactivate() override;
    bool isOn() override;
    const char* getName() override;
};

#endif // RELAY_ACTUATOR_H
