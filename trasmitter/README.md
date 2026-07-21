# ESP32 Transmitter

## Description

The transmitter node collects environmental data inside the chicken coop.

Sensors:

- MQ6 (LPG)
- MQ7 (CO)
- DHT22

After processing, data are transmitted to the receiver using ESP-NOW every 2 seconds.

---

## Hardware

| Device | Pin |
|---------|----|
| MQ6 | GPIO32 |
| MQ7 | GPIO33 |
| DHT22 | GPIO4 |
| Fan Relay | GPIO18 |
| Valve Relay | GPIO19 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |

---

## Data Packet

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

## Sensor Workflow

```
MQ6
MQ7
DHT22
    │
    ▼
Read ADC
    │
Calibration
    │
Gas Decision
    │
Relay Control
    │
ESP-NOW Send
```

---

## MQ6 Calibration

MQ6 was calibrated using a commercial gas detector.

Linear calibration equation:

```
ppm = ((ADC - 1968) × 1131.2) / 1498
```

---

## MQ7 Calibration

MQ7 uses piecewise linear interpolation.

Calibration points:

| ADC | CO (ppm) |
|-----|----------|
|215|0|
|217|11|
|220|14|
|222|16|
|225|17|
|226|19|
|227|21|
|228|22|
|230|23|
|232|25|
|235|27|
|237|28|
|239|30|
|242|36|
|249|43|
|256|57|
|262|121|
|281|197|

---

## Relay Logic

Gas exceeds threshold

↓

Fan ON

Valve CLOSED

Otherwise

↓

Fan OFF

Valve OPEN

---

## LCD Display

Displays

- Countdown
- LPG concentration
- CO concentration
- Temperature
- Send Status
- Packet Number

---

## Sending Interval

2 Seconds

---

## Communication

Protocol

ESP-NOW

Channel

Same as receiver WiFi channel

---

## Output

Every transmission contains

- LPG
- CO
- Temperature
- Alarm Status
- Fan Status
- Valve Status
- Packet Number
