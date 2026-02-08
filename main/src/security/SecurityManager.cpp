#include "security/SecurityManager.h"
#include <Preferences.h>
#include "mbedtls/aes.h"

// Example AES Key (Should be stored securely or derived)
// 16 bytes for AES-128
unsigned char aes_key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

Preferences preferences;

void SecurityManager::init() {
    preferences.begin("poultry_config", false); // Read-Write
    // logic to init mbedtls if needed
}

String SecurityManager::encrypt(String payload) {
    #ifdef ENABLE_ENCRYPTION
        // Simple mock encryption for demonstration.
        // In production, use mbedtls_aes_crypt_ecb / cbc properly with padding.
        // Returning base64 encoded string of encrypted data is standard.
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
    // return preferences.getString("ssid", WIFI_SSID).c_str();
    // Returning macro for now as c_str() on temporary string is dangerous
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
