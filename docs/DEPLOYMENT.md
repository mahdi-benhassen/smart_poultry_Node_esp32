# Deployment Guide

## 1. Hardware Preparation
Ensure your PCB or Breadboard is wired according to `HARDWARE.md`.
*   Power Supply: 5V/2A recommended.
*   Antenna: Ensure WiFi antenna is connected (if using external).

## 2. Firmware Flashing
### Initial Flash (Factory)
1.  Download `poultry_system.bin`, `bootloader.bin`, and `partition-table.bin` from GitHub Releases.
2.  Use **Esptool** or the **ESP32 Flash Download Tool**:
    *   `0x1000`: `bootloader.bin`
    *   `0x8000`: `partition-table.bin`
    *   `0x10000`: `poultry_system.bin`

### Over-The-Air (OTA) Updates
1.  Host the new firmware binary (`poultry_system.bin`) on an HTTPS server.
2.  Update `FIRMWARE_UPDATE_URL` in `Config.h` to point to this URL.
3.  The device will check this URL on boot (or via MQTT command) and self-update.

## 3. WiFi Provisioning
The device ships with no hardcoded WiFi credentials.
1.  Power on the device.
2.  Install the **Espressif ESPTouch** app (iOS/Android).
3.  Connect your phone to the target WiFi network (2.4GHz).
4.  Open App -> Enter WiFi Password -> "Confirm".
5.  The device will receive credentials, save them to NVS, and connect automatically.

## 4. Partition Table
We use a custom partition table (`partitions.csv`) to accommodate the firmware size (~1.2MB).
*   **Factory App**: 2MB
*   **NVS**: 16KB
*   **OTA Data**: 8KB

## 5. Troubleshooting
*   **Boot Loop**: Check power supply stability (brownouts).
*   **WiFi Fails**: Ensure 2.4GHz network (ESP32 does not support 5GHz).
*   **MQTT Disconnect**: Check Broker URL/Port and Firewall rules.
