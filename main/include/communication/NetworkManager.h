#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "Config.h"

class NetworkManager {
private:
    WiFiClientSecure espClient;
    PubSubClient client;
    char mqtt_server[40];
    int mqtt_port;
    WiFiManager wifiManager;
    SemaphoreHandle_t mqttMutex;

public:
    NetworkManager();
    void setupWiFi();
    void setServer(const char* server, int port);
    void connectMQTT();
    void setupOTA();
    void loop();
    void publish(const char* topic, const char* payload);
    bool isConnected();
};

#endif // NETWORK_MANAGER_H
