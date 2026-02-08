idf_component_register(
    SRCS "src/main.cpp"
         "src/actuators/RelayActuator.cpp"
         "src/communication/LoRaManager.cpp"
         "src/communication/NetworkManager.cpp"
         "src/security/SecurityManager.cpp"
         "src/sensors/BH1750Sensor.cpp"
         "src/sensors/DHTSensor.cpp"
         "src/sensors/MQ135Sensor.cpp"
         "src/sensors/UltrasonicSensor.cpp"
         "src/sensors/WaterLevelSensor.cpp"
         "src/system/AutomationController.cpp"
         "src/system/Logger.cpp"
         "src/system/SystemManager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES "esp_wifi"
             "nvs_flash"
             "mqtt"
             "driver"
             "esp_event"
             "esp_netif"
             "esp_log"
             "freertos"
             arduino-LoRa
             ArduinoJson
             MQ135
             WiFiManager
)
