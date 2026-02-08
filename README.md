# Smart Poultry System (ESP32 IoT)

[![Build ESP32 Firmware](https://github.com/mahdi-benhassen/smart_poultry_Node_esp32/actions/workflows/idf_build.yml/badge.svg)](https://github.com/mahdi-benhassen/smart_poultry_Node_esp32/actions/workflows/idf_build.yml)

An advanced IoT firmware for poultry farm automation, built with ESP-IDF and C++.

## Documentation
Full documentation is available in the `docs/` folder:
*   [**Architecture**](docs/ARCHITECTURE.md): System design and software layers.
*   [**Developer Guide**](docs/DEVELOPER_GUIDE.md): Setup, building, and coding standards.
*   [**Deployment Guide**](docs/DEPLOYMENT.md): Flashing, Provisioning, and OTA.
*   [**API Reference**](docs/API_REFERENCE.md): MQTT topics and payloads.
*   [**Hardware Reference**](docs/HARDWARE.md): Pinout and wiring.

## Features
*   **Real-time Monitoring**: Temperature, Humidity, Ammonia, Feed Level.
*   **Automation**: Fan/Heater control logic.
*   **Connectivity**: WiFi (SmartConfig), MQTT, OTA Updates.
*   **Resilience**: Offline data buffering and watchdog protection.

## Quick Start
1.  **Clone**: `git clone ...`
2.  **Build**: `idf.py build`
3.  **Flash**: `idf.py flash monitor`
4.  **Provision**: Use "ESPTouch" app to set WiFi.

## License
MIT
