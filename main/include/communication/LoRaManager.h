#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "Config.h"
#include <string>

// Typedef String to std::string for compatibility if needed, or replace usage
typedef std::string String;

struct LoRaConfig {
    long frequency;
    int spreadingFactor;
    long bandwidth;
    int codingRate;
    int syncWord;
    int txPower;
};

class LoRaManager {
private:
    static SemaphoreHandle_t loraMutex;
    static LoRaConfig config;
public:
    static void init();
    static void sendPacket(String data);
    static void receivePacket(int packetSize);
    static int parsePacket(); // Helper to check packet safely
    
    // Config methods
    static void setConfig(const LoRaConfig& cfg);
    static LoRaConfig getConfig();
    static void loadConfig();
    static void saveConfig();
};

#endif // LORA_MANAGER_H
