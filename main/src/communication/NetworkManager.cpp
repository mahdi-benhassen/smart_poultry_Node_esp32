#include "communication/NetworkManager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_smartconfig.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

static const char *TAG = "NetworkManager";

static void smartconfig_event_handler(void* arg, esp_event_base_t event_base,
                                      int32_t event_id, void* event_data)
{
    if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:%s", rvd_data);
        }

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits((EventGroupHandle_t)arg, BIT0); // Signal done if using event groups
        // Or just stop it here
        esp_smartconfig_stop();
        ESP_LOGI(TAG, "SmartConfig Complete");
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Don't connect immediately if we want to check config first, 
        // but typically we try to connect. If fails, we might start SmartConfig.
        // For now, let's try connect.
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        NetworkManager* nm = (NetworkManager*)arg;
        if (nm) nm->connected = false;
        
        if (nm && !nm->isProvisioning()) {
            esp_wifi_connect();
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
             ESP_LOGI(TAG, "Disconnected (Provisioning Mode or stopped)");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        NetworkManager* nm = (NetworkManager*)arg;
        if (nm) {
             nm->connected = true;
             // We should also potentially reconnect MQTT here if it doesn't auto-reconnect
             // esp-mqtt usually handles reconnects if network is available
             nm->flushQueue(); 
        }
    }
}

NetworkManager::NetworkManager() : mqtt_client(NULL), connected(false), provisioning(false) {
    queueMutex = xSemaphoreCreateMutex();
}

void NetworkManager::startSmartConfig() {
    provisioning = true;
    ESP_LOGI(TAG, "Starting SmartConfig...");
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    
    // Register SmartConfig Handler
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &smartconfig_event_handler, NULL) );
}

void NetworkManager::init() {
    ESP_LOGI(TAG, "Initializing Network...");

    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to init netif: %s", esp_err_to_name(err)); return; }

    err = esp_event_loop_create_default();
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(err)); return; }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to init wifi: %s", esp_err_to_name(err)); return; }

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    
    // Pass 'this' as arg to handle state in callbacks if needed
    err = esp_event_handler_instance_register(WIFI_EVENT,
                                              ESP_EVENT_ANY_ID,
                                              &wifi_event_handler,
                                              this,
                                              &instance_any_id);
    if (err != ESP_OK) ESP_LOGW(TAG, "Failed to register WiFi handler");

    err = esp_event_handler_instance_register(IP_EVENT,
                                              IP_EVENT_STA_GOT_IP,
                                              &wifi_event_handler,
                                              this,
                                              &instance_got_ip);
    if (err != ESP_OK) ESP_LOGW(TAG, "Failed to register IP handler");

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to set mode: %s", esp_err_to_name(err)); return; }

    // Check if we have saved config
    wifi_config_t wifi_config;
    err = esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
    
    bool has_config = (strlen((char*)wifi_config.sta.ssid) > 0);
    
    // If we have compile-time overrides, use them (Dev Mode)
    #ifdef WIFI_SSID
    snprintf((char*)wifi_config.sta.ssid, 32, "%s", WIFI_SSID);
    #endif
    #ifdef WIFI_PASSWORD
    snprintf((char*)wifi_config.sta.password, 64, "%s", WIFI_PASSWORD);
    #endif
    
    // Check again after macro override
    if (strlen((char*)wifi_config.sta.ssid) > 0) {
        ESP_LOGI(TAG, "Found saved/configured SSID: %s", wifi_config.sta.ssid);
        has_config = true;
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    }

    err = esp_wifi_start();
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to start wifi: %s", esp_err_to_name(err)); return; }

    if (!has_config) {
        ESP_LOGI(TAG, "No WiFi config found. Starting SmartConfig...");
        startSmartConfig();
    } else {
        ESP_LOGI(TAG, "Connecting to WiFi...");
        // wifi_event_handler will trigger connect on STA_START
    }

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // MQTT Init
    #ifdef ENABLE_MQTT
    esp_mqtt_client_config_t mqtt_cfg = {};
    #ifdef MQTT_SERVER
    mqtt_cfg.broker.address.uri = MQTT_SERVER; // Ensure MQTT_SERVER includes scheme e.g. "mqtt://broker.com"
    // If MQTT_SERVER is just domain, prepend scheme
    // For now assuming full URI or we construct it.
    // Let's use a safer approach if needed.
    #else
    mqtt_cfg.broker.address.uri = "mqtt://broker.hivemq.com";
    #endif
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to init MQTT client");
    } else {
        err = esp_mqtt_client_start(mqtt_client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(err));
        }
    }
    #endif
}

void NetworkManager::publish(const char* topic, const char* payload) {
    if (mqtt_client && connected) {
        // Try to publish
        int msg_id = esp_mqtt_client_publish(mqtt_client, topic, payload, 0, 1, 0);
        if (msg_id == -1) {
            ESP_LOGW(TAG, "Failed to publish, queueing message...");
            // If failed (and not just disconnected), queue it? 
            // Usually -1 means enqueue failed or client not init.
            // But we trust 'connected' flag mostly.
        } else {
            return; // Success
        }
    }

    // If we are here, we are either not connected or publish failed.
    // Add to Offline Buffer
    if (xSemaphoreTake(queueMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (offlineQueue.size() >= MAX_QUEUE_SIZE) {
            offlineQueue.pop_front(); // Drop oldest
            ESP_LOGW(TAG, "Queue full, dropping oldest message");
        }
        offlineQueue.push_back({std::string(topic), std::string(payload)});
        xSemaphoreGive(queueMutex);
    }
}

void NetworkManager::flushQueue() {
    if (xSemaphoreTake(queueMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        if (!offlineQueue.empty()) {
             ESP_LOGI(TAG, "Flushing %d offline messages...", offlineQueue.size());
             while (!offlineQueue.empty() && connected) {
                 auto msg = offlineQueue.front();
                 int msg_id = esp_mqtt_client_publish(mqtt_client, msg.first.c_str(), msg.second.c_str(), 0, 1, 0);
                 if (msg_id != -1) {
                     offlineQueue.pop_front();
                 } else {
                     // If publish fails here, stop flushing and wait for next chance
                     break; 
                 }
                 vTaskDelay(pdMS_TO_TICKS(10)); // Slight delay to prevent flooding
             }
        }
        xSemaphoreGive(queueMutex);
    }
}

void NetworkManager::checkOTAUpdate() {
    if (!connected) return;

    ESP_LOGI(TAG, "Checking for OTA Update...");
    
    // In a real scenario, we would check a manifest or version file first.
    // Here we assume a direct URL to the binary.
    // Replace with your actual firmware URL
    #define OTA_URL "https://example.com/firmware.bin"
    
    esp_http_client_config_t config = {};
    config.url = OTA_URL;
    config.crt_bundle_attach = esp_crt_bundle_attach; // Use built-in certs
    config.skip_cert_common_name_check = true; // For testing only!

    esp_https_ota_config_t ota_config = {};
    ota_config.http_config = &config;

    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Update successful! Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "OTA Update failed");
    }
}

bool NetworkManager::isConnected() {
    return connected;
}
