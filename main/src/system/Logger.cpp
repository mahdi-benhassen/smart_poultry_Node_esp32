#include "system/Logger.h"
#include <stdarg.h>

SemaphoreHandle_t Logger::serialMutex = NULL;

void Logger::init(long baudRate) {
    Serial.begin(baudRate);
    if (serialMutex == NULL) {
        serialMutex = xSemaphoreCreateMutex();
    }
}

void Logger::log(const char* level, const char* msg) {
    if (serialMutex != NULL) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
            Serial.print("[");
            Serial.print(millis());
            Serial.print("] [");
            Serial.print(level);
            Serial.print("] ");
            Serial.println(msg);
            xSemaphoreGive(serialMutex);
        }
    }
}

void Logger::log(const char* level, String msg) {
    log(level, msg.c_str());
}

void Logger::logf(const char* level, const char* format, ...) {
    if (serialMutex != NULL) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
            Serial.print("[");
            Serial.print(millis());
            Serial.print("] [");
            Serial.print(level);
            Serial.print("] ");
            
            va_list args;
            va_start(args, format);
            vfprintf(stdout, format, args); // Standard printf redirection might not work on all cores, usually Serial.printf
            // Let's use a buffer for safety on embedded
            char buffer[256];
            vsnprintf(buffer, sizeof(buffer), format, args);
            Serial.println(buffer);
            va_end(args);
            
            xSemaphoreGive(serialMutex);
        }
    }
}
