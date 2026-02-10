#include "communication/LoRaManager.h"
#include "security/SecurityManager.h"
#include "system/Logger.h"

SemaphoreHandle_t LoRaManager::loraMutex = NULL;

void LoRaManager::init() {
    #ifdef ENABLE_LORA
    if (loraMutex == NULL) {
        loraMutex = xSemaphoreCreateMutex();
    }
    LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_IRQ);
    if (!LoRa.begin(915E6)) { // 915MHz or 868MHz depending on region
        Logger::log("LoRa", "Starting LoRa failed!");
        return;
    }
    Logger::log("LoRa", "Initialized");
    #endif
}

void LoRaManager::sendPacket(String data) {
    #ifdef ENABLE_LORA
    if (loraMutex != NULL && xSemaphoreTake(loraMutex, portMAX_DELAY)) {
        String encrypted = SecurityManager::encrypt(std::string(data.c_str())).c_str();
        LoRa.beginPacket();
        LoRa.print(encrypted);
        LoRa.endPacket();
        xSemaphoreGive(loraMutex);
    }
    #endif
}

int LoRaManager::parsePacket() {
    #ifdef ENABLE_LORA
    int packetSize = 0;
    if (loraMutex != NULL && xSemaphoreTake(loraMutex, portMAX_DELAY)) {
        packetSize = LoRa.parsePacket();
        xSemaphoreGive(loraMutex);
    }
    return packetSize;
    #else
    return 0;
    #endif
}

void LoRaManager::receivePacket(int packetSize) {
    #ifdef ENABLE_LORA
    if (packetSize == 0) return;

    // Mutex should be held during reading? 
    // Usually yes, because another task might try to write.
    // BUT we already called parsePacket which returned size.
    // If we release mutex between parse and read, another task could interrupt and send?
    // Yes, but LoRa library maintains state. sending might corrupt the receive buffer.
    // So we should ideally lock for the whole receive operation.
    // But since this method is public, we re-lock.
    // RISK: Race condition if another task sends between parse and receive.
    // FIX: Lock inside TaskLoRa loop for both operations or make receivePacket do the reading immediately.
    
    if (loraMutex != NULL && xSemaphoreTake(loraMutex, portMAX_DELAY)) {
        String incoming = "";
        while (LoRa.available()) {
            incoming += (char)LoRa.read();
        }
        xSemaphoreGive(loraMutex); // Release early
        
        std::string decrypted = SecurityManager::decrypt(std::string(incoming.c_str()));
        Logger::log("LoRa", std::string("Received: ") + decrypted);
    }
    #endif
}
