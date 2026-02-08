# Hardware Reference

## 1. Pinout (ESP32-WROOM-32)

| Component | Pin (GPIO) | Type | Notes |
| :--- | :--- | :--- | :--- |
| **I2C Bus** | | | Shared Bus |
| SDA | 21 | I/O | SHT31, BH1750 |
| SCL | 22 | Output | 4.7k Pull-up recommended |
| **Sensors** | | | |
| DHT22 | 25 | I/O | 10k Pull-up required |
| MQ137 | 34 | Analog | Ammonia |
| MQ2 | 35 | Analog | Smoke/Gas |
| PIR | 27 | Input | Motion |
| Door Switch | 12 | Input | Reed Switch |
| **Actuators** | | | Active High/Low depends on Relay |
| Relay Fan | (Config) | Output | |
| Relay Heat | (Config) | Output | |

## 2. Power Requirements
*   **Input Voltage**: 5V DC (via USB or Vin).
*   **Current**: 
    *   Idle: ~80mA
    *   WiFi Tx: ~240mA
    *   Relays Active: +50-100mA per relay.
*   **Recommendation**: Use a robust 5V/2A power supply.

## 3. Wiring Diagrams
*(Placeholder for diagrams)*

### DHT22
```
VCC (1) ---- 3.3V
DATA(2) ---- GPIO 25 (with 10k Pull-up to 3.3V)
NC  (3)
GND (4) ---- GND
```

### Relays
Connect Relay Inputs to configured GPIOs. Ensure flyback diodes are present if driving coils directly (most modules have optocouplers).
