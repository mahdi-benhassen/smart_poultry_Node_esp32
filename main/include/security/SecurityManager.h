#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <string>
#include "Config.h"
#include "system/DataHub.h"

class SecurityManager {
private:
    DataHub* dataHub;

public:
    SecurityManager(DataHub* hub);

    // Logic
    void process(); // Run in SafetyTask or LogicTask
    void arm();
    void disarm();

    // Static Utils (Config/Crypto)
    static void initPreferences(); // Renamed from init() to avoid confusion
    static std::string encrypt(std::string payload);
    static std::string decrypt(std::string payload);
    static const char* getWifiSSID();
    static const char* getWifiPassword();
    static void saveMQTT(const char* server, int port);
    static std::string getMQTTServer();
    static int getMQTTPort();
};

#endif // SECURITY_MANAGER_H
