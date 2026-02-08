#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "Config.h"

#include <deque>
#include <string>

class NetworkManager {
private:
    esp_mqtt_client_handle_t mqtt_client;
    bool connected;
    bool provisioning;
    
    // Offline Buffer
    std::deque<std::pair<std::string, std::string>> offlineQueue;
    const size_t MAX_QUEUE_SIZE = 50;
    SemaphoreHandle_t queueMutex;

    void flushQueue();

public:
    NetworkManager();
    void init();
    void publish(const char* topic, const char* payload);
    bool isConnected();
    
    // Provisioning
    void startSmartConfig();
    bool isProvisioning() { return provisioning; }

    // Friend functions for callbacks
    friend void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    // OTA
    void checkOTAUpdate();
};

#endif // NETWORK_MANAGER_H
