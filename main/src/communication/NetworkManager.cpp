#include "communication/NetworkManager.h"
#include "security/SecurityManager.h"
#include "system/Logger.h"

// Flag for saving data
bool shouldSaveConfig = false;

// Callback notifying us of the need to save config
void saveConfigCallback () {
  Logger::log("NET", "Should save config");
  shouldSaveConfig = true;
}

NetworkManager::NetworkManager() : client(espClient) {
    // Initial default
    strcpy(mqtt_server, MQTT_SERVER);
    mqtt_port = MQTT_PORT;
    mqttMutex = xSemaphoreCreateMutex();
}

void NetworkManager::setupWiFi() {
    Logger::log("NET", "Starting WiFiManager...");

    // Load custom values from SecurityManager (Preferences)
    String saved_server = SecurityManager::getMQTTServer();
    int saved_port = SecurityManager::getMQTTPort();
    
    strcpy(mqtt_server, saved_server.c_str());
    mqtt_port = saved_port;

    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    char port_str[6];
    sprintf(port_str, "%d", mqtt_port);

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", port_str, 6);

    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);

    // Fetches ssid and pass and tries to connect
    // If it does not connect it starts an access point with the specified name
    // and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect("Poultry_Node_AP", "password")) {
        Logger::log("NET", "failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(5000);
    }

    // if you get here you have connected to the WiFi
    Logger::log("NET", "connected...yeey :)");
    Logger::logf("NET", "local ip: %s", WiFi.localIP().toString().c_str());

    // read updated parameters
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    mqtt_port = atoi(custom_mqtt_port.getValue());

    // save the custom parameters to FS
    if (shouldSaveConfig) {
        Logger::log("NET", "Saving config");
        SecurityManager::saveMQTT(mqtt_server, mqtt_port);
    }

    // Configure TLS
    #ifdef ENABLE_TLS
        espClient.setInsecure(); // Skip verification for demo/testing
    #endif
}

void NetworkManager::setupOTA() {
    ArduinoOTA.setHostname("poultry-node");
    
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
        else type = "filesystem";
        Logger::log("OTA", "Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Logger::log("OTA", "\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100))); // Reduce spam
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Logger::logf("OTA", "Error[%u]", error);
        if (error == OTA_AUTH_ERROR) Logger::log("OTA", "Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Logger::log("OTA", "Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Logger::log("OTA", "Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Logger::log("OTA", "Receive Failed");
        else if (error == OTA_END_ERROR) Logger::log("OTA", "End Failed");
    });
    ArduinoOTA.begin();
    Logger::log("OTA", "Ready");
}

void NetworkManager::setServer(const char* server, int port) {
    if (server != mqtt_server) {
        strncpy(mqtt_server, server, 39);
        mqtt_server[39] = '\0';
    }
    mqtt_port = port;
    if (xSemaphoreTake(mqttMutex, portMAX_DELAY)) {
        client.setServer(mqtt_server, mqtt_port);
        xSemaphoreGive(mqttMutex);
    }
}

void NetworkManager::connectMQTT() {
    // Only access client if we have the mutex? 
    // connectMQTT is called from loop(), which should hold mutex or be the only one.
    // However, loop() calls connectMQTT().
    // Let's rely on the caller or lock inside loop.
    // Wait, client.connect() is blocking.
    // If we lock inside loop(), other tasks waiting to publish will block. That's fine.
    
    // For setServer:
    if (xSemaphoreTake(mqttMutex, portMAX_DELAY)) {
         client.setServer(mqtt_server, mqtt_port);
         xSemaphoreGive(mqttMutex);
    }

    // Check connection without lock first to avoid constant locking if connected?
    // client.connected() is fast.
    
    if (!client.connected()) {
        if (xSemaphoreTake(mqttMutex, portMAX_DELAY)) {
             if (!client.connected()) { // Double check
                Logger::logf("MQTT", "Attempting connection to %s:%d", mqtt_server, mqtt_port);
                String clientId = "ESP32Client-";
                clientId += String(random(0xffff), HEX);
                
                if (client.connect(clientId.c_str())) {
                    Logger::log("MQTT", "connected");
                } else {
                    Logger::logf("MQTT", "failed, rc=%d try again in 5s", client.state());
                    // delay(5000); // Don't delay inside lock!
                }
             }
             xSemaphoreGive(mqttMutex);
        }
        if (!client.connected()) delay(5000); // Delay outside lock
    }
}

void NetworkManager::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        // Serial.println("WiFi lost!"); // Handled by reconnect logic eventually
    }

    // Handle OTA
    ArduinoOTA.handle();

    if (!client.connected()) {
        connectMQTT();
    }
    
    if (xSemaphoreTake(mqttMutex, portMAX_DELAY)) {
        client.loop();
        xSemaphoreGive(mqttMutex);
    }
}

void NetworkManager::publish(const char* topic, const char* payload) {
    if (xSemaphoreTake(mqttMutex, portMAX_DELAY)) {
        if (client.connected()) {
            client.publish(topic, payload);
        }
        xSemaphoreGive(mqttMutex);
    }
}

bool NetworkManager::isConnected() {
    return client.connected();
}
