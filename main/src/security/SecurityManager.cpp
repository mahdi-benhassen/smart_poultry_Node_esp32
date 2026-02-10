#include "security/SecurityManager.h"
#include <Preferences.h>
#include "mbedtls/aes.h"
#include "esp_log.h"

static const char* TAG = "SecurityManager";

// Example AES Key (Should be stored securely or derived)
unsigned char aes_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

Preferences preferences;

SecurityManager::SecurityManager(DataHub* hub) : dataHub(hub) {}

void SecurityManager::initPreferences() {
    preferences.begin("poultry_config", false); // Read-Write
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

String SecurityManager::encrypt(String payload) {
    #ifdef ENABLE_ENCRYPTION
        // Simple mock encryption
        return "ENC:" + payload; 
    #else
        return payload;
    #endif
}

String SecurityManager::decrypt(String payload) {
    #ifdef ENABLE_ENCRYPTION
        if (payload.startsWith("ENC:")) {
            return payload.substring(4);
        }
        return payload;
    #else
        return payload;
    #endif
}

const char* SecurityManager::getWifiSSID() {
    // In real usage, read from Preferences
    return WIFI_SSID; 
}

const char* SecurityManager::getWifiPassword() {
    return WIFI_PASSWORD;
}

void SecurityManager::saveMQTT(const char* server, int port) {
    preferences.putString("mqtt_server", server);
    preferences.putInt("mqtt_port", port);
}

String SecurityManager::getMQTTServer() {
    return preferences.getString("mqtt_server", MQTT_SERVER);
}

int SecurityManager::getMQTTPort() {
    return preferences.getInt("mqtt_port", MQTT_PORT);
}
