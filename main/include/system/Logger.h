#ifndef LOGGER_H
#define LOGGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string>

class Logger {
public:
    static void init(long baudRate);
    static void log(const char* level, const char* msg);
    static void log(const char* level, std::string msg);
    static void logf(const char* level, const char* format, ...);
};

#endif // LOGGER_H
