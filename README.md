# ChickAlert - Smart Chicken Coop Monitoring System

ChickAlert is an IoT-based monitoring system for poultry houses using ESP32 and ESP-NOW communication. The system monitors:

- LPG Gas (MQ6)
- Carbon Monoxide (MQ7)
- Temperature (DHT22)

The transmitter node sends sensor data wirelessly to the receiver node through ESP-NOW. The receiver acts as the control room by displaying data, activating alarms, and uploading information to the Blynk Cloud for remote monitoring.

---

## Repository Structure

```
transmitter/
    README.md
    transmitter.ino

receiver/
    README.md
    receiver.ino
```

---

## Hardware

### Transmitter

- ESP32
- MQ6
- MQ7
- DHT22
- LCD 16x2 I2C
- Relay Fan
- Relay Solenoid Valve

### Receiver

- ESP32
- Active Buzzer
- Blynk IoT

---

## Communication

```
MQ6
MQ7
DHT22
      │
      ▼
 ESP32 Transmitter
      │
   ESP-NOW
      │
      ▼
 ESP32 Receiver
      │
 ├── Buzzer
 ├── Serial Monitor
 └── Blynk Cloud
```

---

## Features

- Wireless ESP-NOW communication
- Real-time gas monitoring
- Temperature monitoring
- Local alarm
- Remote monitoring using Blynk
- Fan & Valve automatic control
- Packet counter
- Communication loss detection

---

## Authors

CCIT Faculty of Engineering
Universitas Indonesia
