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
    REQUIRES arduino
             Adafruit_Sensor
             DHT-sensor-library
             BH1750
             pubsubclient
             arduino-LoRa
             ArduinoJson
             MQ135
             WiFiManager
)
