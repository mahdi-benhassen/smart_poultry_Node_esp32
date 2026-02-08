# System Architecture

## 1. Overview
The **Smart Poultry System** is an ESP32-based IoT solution designed for monitoring and automating poultry farm environments. It leverages FreeRTOS for multitasking, MQTT for real-time data telemetry, and a modular C++ architecture for sensor/actuator management.

## 2. Hardware Architecture
The system is built around the **ESP32-WROOM-32** microcontroller.

### Core Components
*   **MCU**: ESP32 (Dual Core, 240MHz, WiFi/BLE)
*   **Connectivity**: WiFi (Station Mode), MQTT (Over TCP/TLS)
*   **Power**: 5V DC Input (regulated to 3.3V)

### Sensor Subsystems
1.  **Environmental**: DHT22 (Temp/Hum), BH1750 (Light), SHT31 (Precision Temp/Hum).
2.  **Air Quality**: MQ137 (Ammonia), MQ2 (Smoke/Gas).
3.  **Resources**: HC-SR04 / JSN-SR04T (Feed/Water Level).
4.  **Security**: PIR (Motion), Door Switches.
5.  **Health**: Thermal Camera (AMG8833 - Optional).

### Actuator Subsystems
*   **Relays**: Control Fans, Heaters, Feeders, and Lights.
*   **Logic**: Automated control based on sensor thresholds (e.g., Turn on Fan if Temp > 30°C).

## 3. Software Architecture
The firmware is developed using **ESP-IDF v5.x** with a C++ Object-Oriented approach.

### Layered Design
1.  **Driver Layer (HAL)**:
    *   Encapsulates hardware specifics (GPIO, I2C, SPI, ADC).
    *   `Sensor` and `Actuator` base classes (Interfaces).
    *   Concrete implementations: `DHTSensor`, `RelayActuator`, etc.

2.  **System Layer**:
    *   `SystemManager`: Orchestrates the entire lifecycle (Init, Task Creation).
    *   `DataHub`: Thread-safe shared memory (Mutex-protected) for exchanging data between tasks.
    *   `LogicEngine`: Rules engine that reads `DataHub` state and triggers `Actuators`.
    *   `Logger`: Centralized logging wrapper.

3.  **Communication Layer**:
    *   `NetworkManager`: Handles WiFi connection (SmartConfig), MQTT connection, and OTA Updates.
    *   **Offline Buffering**: RAM-based ring buffer to store messages when disconnected.

4.  **Application Layer (FreeRTOS Tasks)**:
    *   `SensingTask`: Polls sensors periodically and updates `DataHub`.
    *   `LogicTask`: Evaluates rules and controls relays.
    *   `CommsTask`: Publishes telemetry to MQTT and checks for OTA.
    *   `SafetyTask`: High-priority monitoring for critical failures.

### Data Flow
1.  **Input**: `SensingTask` reads raw sensor values -> Normalizes them -> Writes to `DataHub`.
2.  **Processing**: `LogicTask` reads `DataHub` -> Applies Thresholds -> Sets `SystemState` -> Commands Actuators.
3.  **Output**: `CommsTask` reads `DataHub` snapshot -> Serializes to JSON -> Publishes via MQTT.

## 4. Key Features
*   **SmartConfig Provisioning**: No hardcoded WiFi credentials. Setup via mobile app.
*   **Resilience**: 
    *   Offline Data Buffering (Deque).
    *   Watchdog Timers.
    *   Critical Sections for timing-sensitive drivers (DHT22).
    *   Graceful error handling (no boot loops).
*   **OTA Updates**: Remote firmware updates via HTTPS.
*   **Configurable**: Compile-time feature flags in `Config.h`.
