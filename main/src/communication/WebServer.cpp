#include "communication/WebServer.h"
#include "communication/WebPage.h"
#include "communication/LoRaManager.h"
#include "esp_log.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "WebServer";

// Helper to get instance from context
static WebServer* get_server(httpd_req_t *req) {
    return (WebServer*)req->user_ctx;
}

WebServer::WebServer(DataHub* hub, SystemManager* sys) : server(NULL), dataHub(hub), sysManager(sys) {}

void WebServer::start() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 4096;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting Web Server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Handlers
        httpd_uri_t index_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = index_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t status_uri = {
            .uri       = "/api/status",
            .method    = HTTP_GET,
            .handler   = api_status_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &status_uri);

        httpd_uri_t wifi_uri = {
            .uri       = "/api/config/wifi",
            .method    = HTTP_POST,
            .handler   = api_config_wifi_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &wifi_uri);
        
        httpd_uri_t lora_post_uri = {
            .uri       = "/api/config/lora",
            .method    = HTTP_POST,
            .handler   = api_config_lora_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &lora_post_uri);
        
        httpd_uri_t lora_get_uri = {
            .uri       = "/api/config/lora",
            .method    = HTTP_GET,
            .handler   = api_config_lora_get_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &lora_get_uri);

        httpd_uri_t control_uri = {
            .uri       = "/api/control",
            .method    = HTTP_POST,
            .handler   = api_control_handler,
            .user_ctx  = this
        };
        httpd_register_uri_handler(server, &control_uri);
    } else {
        ESP_LOGE(TAG, "Error starting server!");
    }
}

void WebServer::stop() {
    if (server) {
        httpd_stop(server);
    }
}

// =================================================================================
// Handlers
// =================================================================================

esp_err_t WebServer::index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t WebServer::api_status_handler(httpd_req_t *req) {
    WebServer* self = get_server(req);
    SystemData data = self->dataHub->getSnapshot();
    
    // Check Actuators
    bool fan = false, heater = false, light = false;
    Actuator* aFan = self->sysManager->getActuator("Fan");
    if (aFan) fan = aFan->isOn();
    Actuator* aHeater = self->sysManager->getActuator("Heater");
    if (aHeater) heater = aHeater->isOn();
    Actuator* aLight = self->sysManager->getActuator("Light"); // Assuming we have Light
    if (aLight) light = aLight->isOn();

    // Create JSON (using raw string formatting to avoid heavy cJSON dependency if desired, 
    // but since we linked cJSON, let's use it for safety or just snprintf for speed)
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
        "{\"temp\":%.1f,\"hum\":%.1f,\"nh3\":%.1f,\"feed\":%.1f,\"fan\":%s,\"heater\":%s,\"light\":%s}", 
        data.temperature, data.humidity, data.ammoniaPPM, data.feedLevel,
        fan ? "true" : "false", heater ? "true" : "false", light ? "true" : "false");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, strlen(buffer));
    return ESP_OK;
}

esp_err_t WebServer::api_config_wifi_handler(httpd_req_t *req) {
    char content[100];
    size_t recv_size = (req->content_len < sizeof(content)) ? req->content_len : sizeof(content) - 1;
    
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) return ESP_FAIL;
    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root) {
        cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
        cJSON *pass = cJSON_GetObjectItem(root, "pass");
        
        if (cJSON_IsString(ssid) && cJSON_IsString(pass)) {
            
            wifi_config_t wifi_config = {};
            // Load current to keep other settings?
            esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
            
            strncpy((char*)wifi_config.sta.ssid, ssid->valuestring, sizeof(wifi_config.sta.ssid));
            strncpy((char*)wifi_config.sta.password, pass->valuestring, sizeof(wifi_config.sta.password));
            
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            
            ESP_LOGI(TAG, "WiFi Updated. Rebooting...");
            httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
            
            // Reboot after a delay
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_restart();
        }
        cJSON_Delete(root);
    }
    
    return ESP_OK;
}

esp_err_t WebServer::api_config_lora_handler(httpd_req_t *req) {
    char content[200];
    size_t recv_size = (req->content_len < sizeof(content)) ? req->content_len : sizeof(content) - 1;
    
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) return ESP_FAIL;
    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root) {
        cJSON *freq = cJSON_GetObjectItem(root, "freq");
        cJSON *sf = cJSON_GetObjectItem(root, "sf");
        cJSON *bw = cJSON_GetObjectItem(root, "bw");
        
        LoRaConfig config = LoRaManager::getConfig();
        if (cJSON_IsNumber(freq)) config.frequency = (long)freq->valuedouble;
        if (cJSON_IsNumber(sf)) config.spreadingFactor = sf->valueint;
        if (cJSON_IsNumber(bw)) config.bandwidth = (long)bw->valuedouble;
        
        LoRaManager::setConfig(config);
        
        cJSON_Delete(root);
        httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_500(req);
    }
    
    return ESP_OK;
}

esp_err_t WebServer::api_config_lora_get_handler(httpd_req_t *req) {
    LoRaConfig config = LoRaManager::getConfig();
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "{\"freq\":%ld,\"sf\":%d,\"bw\":%ld}", 
        config.frequency, config.spreadingFactor, config.bandwidth);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buffer, strlen(buffer));
    return ESP_OK;
}

esp_err_t WebServer::api_control_handler(httpd_req_t *req) {
    WebServer* self = get_server(req);
    char content[100];
    size_t recv_size = (req->content_len < sizeof(content)) ? req->content_len : sizeof(content) - 1;
    
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) return ESP_FAIL;
    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root) {
        cJSON *device = cJSON_GetObjectItem(root, "device");
        cJSON *action = cJSON_GetObjectItem(root, "action");
        
        if (cJSON_IsString(device) && cJSON_IsString(action)) {
            Actuator* act = self->sysManager->getActuator(device->valuestring);
            if (act) {
                // Set Manual Mode
                act->setManualMode(true);
                if (strcmp(action->valuestring, "on") == 0) {
                    act->activate();
                } else {
                    act->deactivate();
                }
                ESP_LOGI(TAG, "Manual Control: %s -> %s", device->valuestring, action->valuestring);
            }
        }
        cJSON_Delete(root);
    }
    
    httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
