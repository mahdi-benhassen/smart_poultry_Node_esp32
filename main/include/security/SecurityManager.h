#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>
#include "Config.h"

class SecurityManager {
public:
    static void init();
    static String encrypt(String payload);
    static String decrypt(String payload);
    static const char* getWifiSSID();
    static const char* getWifiPassword();
    static void saveMQTT(const char* server, int port);
    static String getMQTTServer();
    static int getMQTTPort();
};

#endif // SECURITY_MANAGER_H
