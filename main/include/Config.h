#ifndef CONFIG_H
#define CONFIG_H

// =================================================================================
// 1. Environmental & Air Quality Sensors (Vital Signs)
// =================================================================================
#define ENABLE_DHT22            1   // Temp/Humidity (Standard)
#define ENABLE_SHT31            0   // Temp/Humidity (High Precision I2C)
#define ENABLE_MQ137            1   // Ammonia (NH3) - Specific for Poultry
#define ENABLE_MHZ19B           0   // CO2 (NDIR)
#define ENABLE_SCD4X            0   // CO2/Temp/Hum (All-in-one)
#define ENABLE_BH1750           1   // Light Intensity (Lux)
#define ENABLE_BME280           0   // Pressure/Temp/Hum (Tunnel Vent Vacuum)

// =================================================================================
// 2. Resource & Inventory Management
// =================================================================================
#define ENABLE_HC_SR04          1   // Feed Level (Basic)
#define ENABLE_JSN_SR04T        0   // Feed Level (Waterproof)
#define ENABLE_HX711_BIN        0   // Feed Bin Weight (Load Cells)
#define ENABLE_YF_S201          0   // Water Flow/Consumption
#define ENABLE_PH_SENSOR        0   // Water Acidity
#define ENABLE_TURBIDITY        0   // Water Quality

// =================================================================================
// 3. Bird Health & Biometrics (Advanced)
// =================================================================================
#define ENABLE_AMG8833          0   // Thermal Camera (8x8) - Fever/Huddling
#define ENABLE_MLX90614         0   // IR Thermometer (Spot check)
#define ENABLE_AUDIO_AI         0   // INMP441/MAX9814 (Distress/Coughing)
#define ENABLE_BIRD_SCALE       0   // Hanging Load Cell (Growth Tracking)

// =================================================================================
// 4. Security & Biosecurity
// =================================================================================
#define ENABLE_PIR              1   // Motion Detection (Intruder)
#define ENABLE_RADAR_RCWL       0   // Microwave Radar (Hidden Security)
#define ENABLE_RFID_RC522       0   // Access Control (Staff Logging)
#define ENABLE_DOOR_SWITCH      1   // Magnetic Reed Switch (Gate Open/Close)

// =================================================================================
// 5. Operational Health (Machine Monitoring)
// =================================================================================
#define ENABLE_ACS712           0   // Fan Current Monitor (Belt break detection)
#define ENABLE_ZMPT101B         0   // Mains Voltage Monitor (Power Failure)
#define ENABLE_MQ2              1   // Smoke/Gas (Fire Safety)

// =================================================================================
// Actuators
// =================================================================================
#define ENABLE_RELAY_FAN        1   // Ventilation
#define ENABLE_RELAY_HEATER     1   // Heating
#define ENABLE_RELAY_FEEDER     1   // Automated Feeder
#define ENABLE_RELAY_LIGHT      1   // Lighting Control

// =================================================================================
// Communication & Security
// =================================================================================
#define ENABLE_WIFI             1
#define ENABLE_MQTT             1
#define ENABLE_LORA             1
#define ENABLE_TLS              1
#define ENABLE_ENCRYPTION       1

// =================================================================================
// System Settings
// =================================================================================
#define SERIAL_BAUD_RATE        115200
#define SENSOR_POLL_INTERVAL_MS 5000

// WiFi - Leave empty to force SmartConfig provisioning
#define WIFI_SSID               "" 
#define WIFI_PASSWORD           ""

// MQTT
#define MQTT_SERVER             "mqtt://broker.hivemq.com" // Public broker for testing
#define MQTT_PORT               1883 // 8883 for TLS

// OTA
#define FIRMWARE_UPDATE_URL     "https://your-domain.com/firmware/poultry_system.bin"

// =================================================================================
// Pin Definitions (ESP32 Standard Mapping)
// =================================================================================
// I2C Bus (SHT31, BH1750, BME280, AMG8833, MLX90614, SCD4x)
#define PIN_I2C_SDA             21
#define PIN_I2C_SCL             22

// SPI Bus (LoRa, RFID)
#define PIN_SPI_MISO            19
#define PIN_SPI_MOSI            23
#define PIN_SPI_SCK             18
#define PIN_LORA_CS             5
#define PIN_LORA_RST            14
#define PIN_LORA_IRQ            26
#define PIN_RFID_CS             4  // Alternate CS

// Analog Inputs (MQ Sensors, ACS712, ZMPT101B)
#define PIN_MQ137               34  // Ammonia
#define PIN_MQ2                 35  // Smoke
#define PIN_ACS712              32  // Fan Current
#define PIN_ZMPT101B            33  // Voltage

// Digital I/O
#define PIN_DHT                 25
#define PIN_PIR                 27
#define PIN_DOOR_SWITCH         12
#define PIN_TRIG                13  // Ultrasonic
#define PIN_ECHO                14  // Ultrasonic

// Relays (Active Low/High depending on module)
#define PIN_RELAY_FAN           15
#define PIN_RELAY_HEATER        2
#define PIN_RELAY_FEEDER        0
#define PIN_RELAY_LIGHT         16

// UART (MH-Z19B, GPS)
#define PIN_UART2_TX            17
#define PIN_UART2_RX            16

// =================================================================================
// Automation Thresholds
// =================================================================================
#define THRESHOLD_TEMP_HIGH     30.0 // Celsius
#define THRESHOLD_TEMP_LOW      25.0 // Celsius
#define THRESHOLD_AMMONIA_MAX   25.0 // PPM (Warning level)
#define THRESHOLD_CO2_MAX       1000 // PPM
#define THRESHOLD_FEED_LOW      20.0 // CM distance
#define THRESHOLD_LUX_MIN       10.0 // Turn on lights

#endif // CONFIG_H
