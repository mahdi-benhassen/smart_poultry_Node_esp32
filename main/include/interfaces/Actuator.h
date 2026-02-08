#ifndef ACTUATOR_INTERFACE_H
#define ACTUATOR_INTERFACE_H

class Actuator {
public:
    virtual ~Actuator() {}
    virtual void init() = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual bool isOn() = 0;
    virtual const char* getName() = 0;
    
    // Manual Override
    virtual void setManualMode(bool manual) = 0;
    virtual bool isManualMode() = 0;
};

#endif // ACTUATOR_INTERFACE_H
