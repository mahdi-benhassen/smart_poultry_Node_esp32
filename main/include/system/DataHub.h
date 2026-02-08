#ifndef DATA_HUB_H
#define DATA_HUB_H

#include "SystemState.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

class DataHub {
private:
    SystemData currentData;
    SemaphoreHandle_t dataMutex;

public:
    DataHub() {
        dataMutex = xSemaphoreCreateMutex();
        // Initialize with safe defaults
        currentData = {0};
        currentData.currentState = HouseState::NORMAL;
    }

    void updateSensorData(const SystemData& newData) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Merge or replace logic. For now, we assume newData is a full update 
            // or we update individual fields. Let's provide specific setters for better granularity.
            // But for simplicity of this prompt, let's assume partial updates are handled by getters/setters.
            
            // Example: Preserve state if not provided in newData? 
            // No, the LogicEngine controls state. Sensors control values.
            
            // Let's implement specific setters for thread safety
            xSemaphoreGive(dataMutex);
        }
    }

    void setTemperature(float temp) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.temperature = temp;
            xSemaphoreGive(dataMutex);
        }
    }

    float getTemperature() {
        float val = 0.0;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            val = currentData.temperature;
            xSemaphoreGive(dataMutex);
        }
        return val;
    }

    void setAmmonia(float ppm) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.ammoniaPPM = ppm;
            xSemaphoreGive(dataMutex);
        }
    }

    float getAmmonia() {
        float val = 0.0;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            val = currentData.ammoniaPPM;
            xSemaphoreGive(dataMutex);
        }
        return val;
    }

    void setLux(float lux) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.lux = lux;
            xSemaphoreGive(dataMutex);
        }
    }

    float getLux() {
        float val = 0.0;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            val = currentData.lux;
            xSemaphoreGive(dataMutex);
        }
        return val;
    }

    void setSmokeLevel(float smoke) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.smokeLevel = smoke;
            xSemaphoreGive(dataMutex);
        }
    }

    float getSmokeLevel() {
        float val = 0.0;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            val = currentData.smokeLevel;
            xSemaphoreGive(dataMutex);
        }
        return val;
    }

    void setMotion(bool motion) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.motionDetected = motion;
            xSemaphoreGive(dataMutex);
        }
    }

    bool getMotion() {
        bool val = false;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            val = currentData.motionDetected;
            xSemaphoreGive(dataMutex);
        }
        return val;
    }

    void setState(HouseState state) {
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            currentData.currentState = state;
            xSemaphoreGive(dataMutex);
        }
    }

    HouseState getState() {
        HouseState s = HouseState::NORMAL;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(10))) {
            s = currentData.currentState;
            xSemaphoreGive(dataMutex);
        }
        return s;
    }

    // Snapshot for marshalling
    SystemData getSnapshot() {
        SystemData snapshot;
        if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100))) {
            snapshot = currentData;
            xSemaphoreGive(dataMutex);
        }
        return snapshot;
    }
};

#endif // DATA_HUB_H
