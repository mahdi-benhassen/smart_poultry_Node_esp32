#include "system/Logger.h"
#include "esp_log.h"
#include <stdarg.h>

void Logger::init(long baudRate) {
    // No-op for ESP-IDF (initialized by default)
}

void Logger::log(const char* level, const char* msg) {
    ESP_LOGI(level, "%s", msg);
}

void Logger::log(const char* level, std::string msg) {
    log(level, msg.c_str());
}

void Logger::logf(const char* level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    // ESP_LOGI uses a macro, so we can't easily pass va_list to it directly in a generic way 
    // without using esp_log_write or buffering.
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    ESP_LOGI(level, "%s", buffer);
    va_end(args);
}
