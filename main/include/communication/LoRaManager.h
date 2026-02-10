#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "Config.h"
#include <string>

// Typedef String to std::string for compatibility if needed, or replace usage
typedef std::string String;

class LoRaManager {
private:
    static SemaphoreHandle_t loraMutex;
public:
    static void init();
    static void sendPacket(String data);
    static void receivePacket(int packetSize);
    static int parsePacket(); // Helper to check packet safely
};

#endif // LORA_MANAGER_H
