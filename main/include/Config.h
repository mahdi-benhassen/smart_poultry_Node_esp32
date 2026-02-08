#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware & Sensor Modules ---
#define ENABLE_DHT_SENSOR       1   // Temp/Humidity
#define ENABLE_MQ135_SENSOR     1   // Ammonia/Air Quality
#define ENABLE_BH1750_SENSOR    1   // Light Intensity
#define ENABLE_ULTRASONIC_SENSOR 1  // Feed Level
#define ENABLE_WATER_LEVEL      1   // Water Level (Float/Non-contact)

// --- Actuator Modules ---
#define ENABLE_RELAY_FAN        1   // Ventilation
#define ENABLE_RELAY_HEATER     1   // Heating
#define ENABLE_RELAY_FEEDER     1   // Automated Feeder

// --- Communication Modules ---
#define ENABLE_WIFI             1
#define ENABLE_MQTT             1
#define ENABLE_LORA             1

// --- Security Modules ---
#define ENABLE_TLS              1   // Transport Security
#define ENABLE_ENCRYPTION       1   // Payload Encryption (AES)

// --- System Settings ---
#define SERIAL_BAUD_RATE        115200
#define SENSOR_POLL_INTERVAL_MS 5000
#define WIFI_SSID               "Poultry_Net"
#define WIFI_PASSWORD           "secure_password"
#define MQTT_SERVER             "mqtt.example.com"
#define MQTT_PORT               8883 // Secure port

// --- Pin Definitions (Example Mapping) ---
#define PIN_DHT                 4
#define PIN_MQ135               34  // Analog Input
#define PIN_BH1750_SDA          21
#define PIN_BH1750_SCL          22
#define PIN_ULTRASONIC_TRIG     32
#define PIN_ULTRASONIC_ECHO     35  // Input Only
#define PIN_WATER_LEVEL         36  // Input Only (VP)
#define PIN_RELAY_FAN           15
#define PIN_RELAY_HEATER        2
#define PIN_RELAY_FEEDER        33
#define PIN_LORA_CS             5
#define PIN_LORA_RESET          27
#define PIN_LORA_IRQ            26

// --- Automation Thresholds ---
#define THRESHOLD_TEMP_HIGH     30.0 // Turn on Fan
#define THRESHOLD_TEMP_LOW      25.0 // Turn on Heater
#define THRESHOLD_AMMONIA_MAX   50.0 // PPM, Turn on Fan/Alarm
#define THRESHOLD_FEED_LOW      10.0 // cm, Trigger Feeder


#endif // CONFIG_H
