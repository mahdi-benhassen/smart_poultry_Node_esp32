#include "communication/LoRaManager.h"
#include "security/SecurityManager.h"
#include "system/Logger.h"

SemaphoreHandle_t LoRaManager::loraMutex = NULL;

void LoRaManager::init() {
    #ifdef ENABLE_LORA
    // TODO: Implement Native LoRa Driver
    Logger::log("LoRa", "LoRa Disabled: No Native Driver Available");
    #endif
}

void LoRaManager::sendPacket(String data) {
    #ifdef ENABLE_LORA
    // Stub
    #endif
}

int LoRaManager::parsePacket() {
    return 0;
}

void LoRaManager::receivePacket(int packetSize) {
    // Stub
}
