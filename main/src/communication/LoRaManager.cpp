#include "communication/LoRaManager.h"
#include "security/SecurityManager.h"
#include "system/Logger.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char* TAG = "LoRaManager";

SemaphoreHandle_t LoRaManager::loraMutex = NULL;
LoRaConfig LoRaManager::config = { 
    .frequency = 433000000, 
    .spreadingFactor = 7, 
    .bandwidth = 125000, 
    .codingRate = 5, 
    .syncWord = 0x12, 
    .txPower = 17 
};

void LoRaManager::init() {
    loadConfig();
    #ifdef ENABLE_LORA
    // TODO: Implement Native LoRa Driver with config
    ESP_LOGI(TAG, "LoRa Config: Freq=%ld, SF=%d", config.frequency, config.spreadingFactor);
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

void LoRaManager::setConfig(const LoRaConfig& cfg) {
    config = cfg;
    saveConfig();
}

LoRaConfig LoRaManager::getConfig() {
    return config;
}

void LoRaManager::loadConfig() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err == ESP_OK) {
        int64_t freq = 0;
        int64_t bw = 0;
        nvs_get_i64(my_handle, "lora_freq", &freq);
        if (freq > 0) config.frequency = (long)freq;
        
        nvs_get_i32(my_handle, "lora_sf", &config.spreadingFactor);
        
        nvs_get_i64(my_handle, "lora_bw", &bw);
        if (bw > 0) config.bandwidth = (long)bw;
        
        nvs_get_i32(my_handle, "lora_cr", &config.codingRate);
        nvs_get_i32(my_handle, "lora_sw", &config.syncWord);
        nvs_get_i32(my_handle, "lora_pwr", &config.txPower);
        nvs_close(my_handle);
    }
}

void LoRaManager::saveConfig() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        nvs_set_i64(my_handle, "lora_freq", config.frequency);
        nvs_set_i32(my_handle, "lora_sf", config.spreadingFactor);
        nvs_set_i64(my_handle, "lora_bw", config.bandwidth);
        nvs_set_i32(my_handle, "lora_cr", config.codingRate);
        nvs_set_i32(my_handle, "lora_sw", config.syncWord);
        nvs_set_i32(my_handle, "lora_pwr", config.txPower);
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}
