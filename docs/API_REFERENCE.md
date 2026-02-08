# API Reference (MQTT)

The system communicates via MQTT. All payloads are in **JSON** format.

## 1. Telemetry
**Topic**: `poultry/status`  
**Direction**: Device -> Broker  
**Frequency**: Periodic (Default: 5s)

### Payload Structure
```json
{
  "system": {
    "state": "NORMAL",      // NORMAL, WARNING, CRITICAL
    "uptime": 12345,        // Seconds
    "wifi_rssi": -65
  },
  "sensors": {
    "temp": 28.5,           // Celsius
    "hum": 65.0,            // %
    "ammonia": 12.5,        // ppm
    "light": 150,           // lux
    "feed_level": 80        // %
  },
  "actuators": {
    "fan": true,
    "heater": false,
    "feeder": false
  }
}
```

## 2. Control (Commands)
**Topic**: `poultry/control`  
**Direction**: Broker -> Device

### Supported Commands
1.  **Force Actuator**:
    ```json
    { "command": "set_fan", "value": true }
    ```
2.  **System Reset**:
    ```json
    { "command": "reboot" }
    ```
3.  **Trigger OTA**:
    ```json
    { "command": "update_firmware" }
    ```

## 3. Provisioning & Setup
*   **SmartConfig**: The device listens for ESPTouch packets when unprovisioned.
*   **Logs**: `poultry/logs` (Optional, if remote logging enabled).
