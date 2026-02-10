#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h"

class DataHub;
class SystemManager;

class WebServer {
private:
    httpd_handle_t server;
    DataHub* dataHub;
    SystemManager* sysManager;

    static esp_err_t index_handler(httpd_req_t *req);
    static esp_err_t api_status_handler(httpd_req_t *req);
    static esp_err_t api_config_wifi_handler(httpd_req_t *req);
    static esp_err_t api_config_lora_handler(httpd_req_t *req);
    static esp_err_t api_config_lora_get_handler(httpd_req_t *req);
    static esp_err_t api_control_handler(httpd_req_t *req);

public:
    WebServer(DataHub* hub, SystemManager* sys);
    void start();
    void stop();
};

#endif // WEB_SERVER_H
