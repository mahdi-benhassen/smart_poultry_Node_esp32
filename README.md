# Smart Poultry Node (ESP32)

A robust, modular IoT firmware for poultry farm monitoring and automation, built for the **ESP32** using the **ESP-IDF** build system with **Arduino Components**.

## Features

*   **Modular Architecture**: Separate managers for Sensors, Actuators, Network, Security, and System logic.
*   **Sensor Support**:
    *   **DHT22**: Temperature & Humidity.
    *   **MQ-135**: Air Quality (Ammonia).
    *   **BH1750**: Light Intensity.
    *   **HC-SR04**: Ultrasonic Feed Level.
    *   **Water Level**: Resistive/Float monitoring.
*   **Connectivity**:
    *   **Wi-Fi**: Auto-provisioning via Captive Portal (`WiFiManager`).
    *   **MQTT**: Secure data publishing (TLS supported).
    *   **LoRa**: Long-range node-to-node communication.
    *   **OTA**: Over-the-Air firmware updates.
*   **Reliability**:
    *   **FreeRTOS**: Multitasking (Sensors, Network, LoRa).
    *   **Thread Safety**: Mutex-protected resources (Logging, MQTT, SPI).
    *   **Watchdog**: Hardware WDT monitoring for all tasks.

## Project Structure

This project uses the standard ESP-IDF component structure:

```text
├── main/
│   ├── src/                # Application source code
│   ├── include/            # Application headers
│   ├── CMakeLists.txt      # Main component build config
│   └── idf_component.yml   # Dependency manager (fetches Arduino Core)
├── components/             # 3rd-party Arduino libraries (vendored)
├── CMakeLists.txt          # Project build config
├── sdkconfig.defaults      # Default SDK configuration
└── setup_libs.ps1          # Helper to fetch libraries
```

## Build Instructions

### Prerequisites
*   [ESP-IDF v5.1+](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) installed and exported to your PATH.

### 1. Clone the Repository
```bash
git clone https://github.com/mahdi-benhassen/smart_poultry_Node_esp32.git
cd smart_poultry_Node_esp32
```

### 2. Configure
```bash
idf.py set-target esp32
idf.py menuconfig
```
*   Ensure `Component config -> Arduino Configuration -> Autostart Arduino setup and loop` is enabled.

### 3. Build & Flash
```bash
idf.py build
idf.py -p COM3 flash monitor
```
*(Replace `COM3` with your device's serial port)*

## Configuration
Edit `main/include/Config.h` to enable/disable modules and set pin mappings:

```cpp
#define ENABLE_DHT_SENSOR       1
#define ENABLE_MQ135_SENSOR     1
#define ENABLE_LORA             1
// ...
```

## Provisioning
1.  On first boot, the device creates a Wi-Fi Access Point named **`Poultry_Node_AP`**.
2.  Connect to it (Password: `password` or open).
3.  A captive portal will open (or go to `192.168.4.1`).
4.  Enter your Wi-Fi credentials and MQTT Broker details.

## License
MIT
