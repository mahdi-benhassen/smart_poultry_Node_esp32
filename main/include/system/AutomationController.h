#ifndef AUTOMATION_CONTROLLER_H
#define AUTOMATION_CONTROLLER_H

#include "SystemManager.h"
#include "Config.h"

class AutomationController {
public:
    static void run(SystemManager& sysManager);
};

#endif // AUTOMATION_CONTROLLER_H
