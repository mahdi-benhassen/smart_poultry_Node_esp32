#include "security/SecurityManager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "mbedtls/aes.h"
#include "esp_log.h"

static const char* TAG = "SecurityManager";

// Example AES Key (Should be stored securely or derived)
unsigned char aes_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

SecurityManager::SecurityManager(DataHub* hub) : dataHub(hub) {}

void SecurityManager::initPreferences() {
    // NVS is initialized in main.cpp
    // This function can be used for specific namespace initialization if needed
}

void SecurityManager::process() {
    SecurityState state = dataHub->getSecurityState();
    
    // 1. Fire Safety (Always Active)
    // We get smoke level directly or from dataHub
    float smoke = dataHub->getSmokeLevel();
    // Assuming smoke > 300 is FIRE
    if (smoke > 300.0) {
        if (state != SecurityState::ALARM_FIRE) {
            ESP_LOGE(TAG, "CRITICAL: FIRE DETECTED! Level: %.2f", smoke);
            dataHub->setSecurityState(SecurityState::ALARM_FIRE);
            // Also trigger HouseState EMERGENCY via LogicEngine or here?
            // LogicEngine handles HouseState based on smoke too, so we are consistent.
        }
        return; // Fire overrides everything
    }

    // 2. Intrusion Detection (Only if ARMED)
    if (state == SecurityState::ARMED) {
        bool motion = dataHub->getMotion();
        if (motion) {
            ESP_LOGW(TAG, "INTRUSION DETECTED! Motion sensor triggered.");
            dataHub->setSecurityState(SecurityState::ALARM_INTRUSION);
        }
    }
}

void SecurityManager::arm() {
    ESP_LOGI(TAG, "System ARMED");
    dataHub->setSecurityState(SecurityState::ARMED);
}

void SecurityManager::disarm() {
    ESP_LOGI(TAG, "System DISARMED");
    dataHub->setSecurityState(SecurityState::DISARMED);
}

// =================================================================================
// Static Utils
// =================================================================================

std::string SecurityManager::encrypt(std::string payload) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, aes_key, 128);

    int len = payload.length();
    int pad = 16 - (len % 16);
    std::string padded = payload;
    padded.append(pad, (char)pad);

    std::string output;
    output.resize(padded.length());

    for (size_t i = 0; i < padded.length(); i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, 
            (const unsigned char*)padded.c_str() + i, 
            (unsigned char*)&output[0] + i);
    }
    
    mbedtls_aes_free(&aes);
    return output;
}

std::string SecurityManager::decrypt(std::string payload) {
    if (payload.length() % 16 != 0) return "";

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 128);

    std::string output;
    output.resize(payload.length());

    for (size_t i = 0; i < payload.length(); i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, 
            (const unsigned char*)payload.c_str() + i, 
            (unsigned char*)&output[0] + i);
    }
    
    mbedtls_aes_free(&aes);

    int pad = output.back();
    if (pad > 0 && pad <= 16) {
        output.resize(output.length() - pad);
    }
    
    return output;
}

const char* SecurityManager::getWifiSSID() {
    // In real usage, read from Preferences
    return WIFI_SSID; 
}

const char* SecurityManager::getWifiPassword() {
    return WIFI_PASSWORD;
}

void SecurityManager::saveMQTT(const char* server, int port) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("poultry_config", NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        nvs_set_str(my_handle, "mqtt_server", server);
        nvs_set_i32(my_handle, "mqtt_port", port);
        nvs_commit(my_handle);
        nvs_close(my_handle);
    }
}

std::string SecurityManager::getMQTTServer() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("poultry_config", NVS_READONLY, &my_handle);
    if (err != ESP_OK) return MQTT_SERVER;

    size_t required_size;
    if (nvs_get_str(my_handle, "mqtt_server", NULL, &required_size) == ESP_OK) {
        char* server = new char[required_size];
        nvs_get_str(my_handle, "mqtt_server", server, &required_size);
        std::string result(server);
        delete[] server;
        nvs_close(my_handle);
        return result;
    }
    nvs_close(my_handle);
    return MQTT_SERVER;
}

int SecurityManager::getMQTTPort() {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("poultry_config", NVS_READONLY, &my_handle);
    if (err != ESP_OK) return MQTT_PORT;

    int32_t port = MQTT_PORT;
    nvs_get_i32(my_handle, "mqtt_port", &port);
    nvs_close(my_handle);
    return (int)port;
}
