#include "communication/NetworkManager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "NetworkManager";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "retry to connect to the AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

NetworkManager::NetworkManager() : mqtt_client(NULL), connected(false) {}

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
    
    err = esp_event_handler_instance_register(WIFI_EVENT,
                                              ESP_EVENT_ANY_ID,
                                              &wifi_event_handler,
                                              NULL,
                                              &instance_any_id);
    if (err != ESP_OK) ESP_LOGW(TAG, "Failed to register WiFi handler");

    err = esp_event_handler_instance_register(IP_EVENT,
                                              IP_EVENT_STA_GOT_IP,
                                              &wifi_event_handler,
                                              NULL,
                                              &instance_got_ip);
    if (err != ESP_OK) ESP_LOGW(TAG, "Failed to register IP handler");

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    
    // Check Config.h macros if available
    #ifdef WIFI_SSID
    snprintf((char*)wifi_config.sta.ssid, 32, "%s", WIFI_SSID);
    #else
    snprintf((char*)wifi_config.sta.ssid, 32, "SSID_PLACEHOLDER");
    #endif

    #ifdef WIFI_PASSWORD
    snprintf((char*)wifi_config.sta.password, 64, "%s", WIFI_PASSWORD);
    #else
    snprintf((char*)wifi_config.sta.password, 64, "PASSWORD_PLACEHOLDER");
    #endif
    
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to set mode: %s", esp_err_to_name(err)); return; }

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to set config: %s", esp_err_to_name(err)); return; }

    err = esp_wifi_start();
    if (err != ESP_OK) { ESP_LOGE(TAG, "Failed to start wifi: %s", esp_err_to_name(err)); return; }

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
    if (mqtt_client) {
        esp_mqtt_client_publish(mqtt_client, topic, payload, 0, 1, 0);
    }
}

bool NetworkManager::isConnected() {
    return connected;
}
