# Developer Guide

## 1. Environment Setup

### Prerequisites
*   **VS Code** with **Espressif IDF Extension**.
*   **ESP-IDF v5.1.x** (installed via extension or manually).
*   **Git**.

### Installation
1.  Clone the repository:
    ```bash
    git clone https://github.com/mahdi-benhassen/smart_poultry_Node_esp32.git
    cd smart_poultry_Node_esp32
    ```
2.  Open in VS Code.
3.  Configure ESP-IDF extension (select kit `esp32`).

## 2. Project Structure
```
├── .github/            # CI/CD Workflows
├── main/
│   ├── CMakeLists.txt  # Component build config
│   ├── include/        # Header files
│   │   ├── sensors/    # Sensor interfaces & implementations
│   │   ├── system/     # Core system logic
│   │   └── Config.h    # Global Configuration
│   └── src/            # Source files (C++)
├── partitions.csv      # Custom partition table (Factory app size)
├── sdkconfig.defaults  # Default ESP-IDF config
└── CMakeLists.txt      # Project build config
```

## 3. Configuration (`Config.h`)
The system is highly configurable via `main/include/Config.h`.
*   **Feature Flags**: Enable/Disable sensors (`ENABLE_DHT22`, `ENABLE_MQTT`, etc.).
*   **Pin Definitions**: Remap GPIOs.
*   **Network Settings**: Set default MQTT broker or OTA URL.

## 4. Building and Flashing

### Build
Run the build command in the terminal or use the VS Code extension button:
```bash
idf.py build
```

### Flash
Connect your ESP32 via USB and run:
```bash
idf.py -p COMx flash monitor
```
*(Replace `COMx` with your serial port)*

### Debugging
*   **Serial Monitor**: Use `idf.py monitor` to see logs.
*   **Core Dumps**: If the device crashes, decode the backtrace using the ELF file.

## 5. Adding a New Sensor
1.  Create `NewSensor.h` inheriting from `Sensor` interface.
2.  Implement `init()`, `read()`, `report()`, and `getName()`.
3.  Add the new file to `main/CMakeLists.txt` sources.
4.  Instantiate and register it in `SystemManager.cpp`.
5.  Add a toggle in `Config.h`.

## 6. Release Process
*   **CI/CD**: Pushing a tag `v*` (e.g., `v1.0.0`) triggers a GitHub Action to build and release the firmware binary.
*   **Artifacts**: Download `poultry_system.bin` from the Releases page.
