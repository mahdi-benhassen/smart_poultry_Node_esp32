#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "Config.h"

class NetworkManager {
private:
    esp_mqtt_client_handle_t mqtt_client;
    bool connected;

public:
    NetworkManager();
    void init();
    void publish(const char* topic, const char* payload);
    bool isConnected();
};

#endif // NETWORK_MANAGER_H
