# ChickAlert - ESP32 Smart Chicken Coop Monitoring System

An IoT-based environmental monitoring system for poultry houses using **ESP32**, **ESP-NOW**, and **Blynk IoT**. The system continuously monitors combustible gas concentration, carbon monoxide concentration, and ambient temperature while providing both local and remote alarms.

---

# Features

- ESP-NOW wireless communication
- Real-time LPG monitoring (MQ6)
- Real-time CO monitoring (MQ7)
- Temperature monitoring (DHT22)
- Automatic fan control
- Automatic gas valve control
- Local audible alarm
- Remote monitoring using Blynk Cloud
- Communication loss detection
- LCD monitoring
- Packet counter

---

# Repository Structure

```
Chicken-Gas-Monitoring-ESPNow/
│
├── transmitter/
│   ├── transmitter.ino
│   └── README.md
│
├── receiver/
│   ├── receiver.ino
│   └── README.md
│
├── LICENSE
│
└── README.md
```

---

# System Architecture

```
                     Chicken Coop

        ┌─────────────────────────────┐
        │                             │
        │          MQ6                │
        │          MQ7                │
        │         DHT22               │
        │                             │
        │      ESP32 Transmitter      │
        │                             │
        │ LCD                         │
        │ Fan Relay                   │
        │ Valve Relay                 │
        └──────────────┬──────────────┘
                       │
                 ESP-NOW Wireless
                       │
                       ▼
        ┌─────────────────────────────┐
        │      ESP32 Receiver         │
        │                             │
        │ Active Buzzer               │
        │ Serial Monitor              │
        │ WiFi                        │
        │                             │
        └──────────────┬──────────────┘
                       │
                    Internet
                       │
                       ▼
                 Blynk Cloud
                       │
                       ▼
                Blynk Mobile App
```

---

# System Workflow

```
Sensor Reading
      │
      ▼
Sensor Calibration
      │
      ▼
Alarm Decision
      │
      ▼
Relay Control
      │
      ▼
ESP-NOW Transmission
      │
      ▼
Receiver Processing
      │
      ├────────► Local Alarm
      │
      └────────► Blynk Cloud
```

---

# Hardware

## Transmitter

- ESP32 DevKit V1
- MQ6 Gas Sensor
- MQ7 Carbon Monoxide Sensor
- DHT22 Temperature Sensor
- LCD 16×2 / 20×4 I2C
- Relay Module
- Exhaust Fan
- Solenoid Valve

## Receiver

- ESP32 DevKit V1
- Active Buzzer
- WiFi Connection

---

# Communication

## Sensor Node

```
Sampling Interval : 2 Seconds
Protocol          : ESP-NOW
Payload           : SensorData Structure
```

## Cloud Communication

```
Protocol : WiFi
Platform : Blynk IoT
```

---

# Data Packet

```cpp
typedef struct
{
    float lpg;
    float co;
    float temp;

    bool gasAlarm;
    bool fanStatus;
    bool valveStatus;

    uint32_t packetNumber;

} SensorData;
```

---

# Alarm Logic

## Gas Alarm

```
LPG ≥ Threshold
```

Action

- Fan ON
- Valve CLOSED
- Fast Buzzer

---

## Low Temperature

```
Temperature < 25°C
```

Action

- Continuous Buzzer
- Cold Coop Warning

---

## Communication Lost

```
No Packet > 10 Seconds
```

Action

- Slow Buzzer
- Receiver OFFLINE

---

# Mathematical Model

## MQ6

Linear calibration

$$
LPG_{ppm}=m(ADC-ADC_0)
$$

---

## MQ7

Piecewise Linear Interpolation

$$
C=C_1+\frac{(ADC-ADC_1)(C_2-C_1)}
{ADC_2-ADC_1}
$$

---

## Temperature

$$
T=T_{raw}+Offset
$$

---

# Project Modules

| Module | Description |
|---------|-------------|
| transmitter | Sensor acquisition, calibration, relay control, LCD display, ESP-NOW transmission |
| receiver | ESP-NOW reception, alarm management, Blynk integration, communication monitoring |

---

# Software Requirements

- Arduino IDE 2.x
- ESP32 Board Package
- Blynk Library

Required libraries

```
WiFi.h
esp_now.h
esp_wifi.h
DHT.h
LiquidCrystal_I2C.h
BlynkSimpleEsp32.h
```

---

# Getting Started

1. Upload the transmitter firmware.
2. Upload the receiver firmware.
3. Configure the receiver WiFi credentials.
4. Configure the Blynk Template.
5. Verify that both ESP32 devices operate on the same WiFi channel.
6. Power both devices.
7. Open the Blynk dashboard to monitor sensor values.

---

# Documentation

Detailed implementation is available in:

- [`transmitter/README.md`](transmitter/README.md)
- [`receiver/README.md`](receiver/README.md)

---

# License

This project is distributed under the MIT License.
