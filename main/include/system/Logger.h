#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class Logger {
private:
    static SemaphoreHandle_t serialMutex;

public:
    static void init(long baudRate);
    static void log(const char* level, const char* msg);
    static void log(const char* level, String msg);
    static void logf(const char* level, const char* format, ...);
};

#endif // LOGGER_H
