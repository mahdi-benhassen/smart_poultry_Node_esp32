#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <LoRa.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "Config.h"

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
