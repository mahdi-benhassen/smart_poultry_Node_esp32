#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "Config.h"
#include "system/DataHub.h"

#include <deque>
#include <string>

#include "communication/WebServer.h"

class SystemManager; // Forward declaration

class NetworkManager {
private:
    DataHub* dataHub;
    SystemManager* sysManager;
    WebServer* webServer;
    esp_mqtt_client_handle_t mqtt_client;
    bool connected;
    bool provisioning;
    
    // Offline Buffer
    std::deque<std::pair<std::string, std::string>> offlineQueue;
    const size_t MAX_QUEUE_SIZE = 50;
    SemaphoreHandle_t queueMutex;

    void flushQueue();

public:
    NetworkManager(DataHub* hub, SystemManager* sys);
    void init();
    void publish(const char* topic, const char* payload);
    bool isConnected();
    
    // Provisioning & AP
    void startSmartConfig();
    void startAP();
    void startWebServer();
    bool isProvisioning() { return provisioning; }

    // Friend functions for callbacks
    friend void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    friend void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

    // OTA
    void checkOTAUpdate();
};

#endif // NETWORK_MANAGER_H
