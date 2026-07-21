# ESP32 Receiver

## Description

The receiver node acts as the monitoring station.

Functions

- Receive ESP-NOW packets
- Display sensor information
- Activate buzzer
- Upload data to Blynk Cloud

---

## Hardware

| Device | Pin |
|---------|----|
| Active Buzzer | GPIO25 |

---

## ESP-NOW Packet

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

## Alarm Logic

### Gas Alarm

Condition

```
gasAlarm == true
```

Action

- Fast buzzer (250 ms)

---

### Low Temperature

Condition

```
Temperature < 25°C
```

Action

- Continuous buzzer
- Cold coop warning

---

### Communication Lost

Condition

```
No packet for 10 seconds
```

Action

- Slow buzzer (1 second)
- Receiver marked OFFLINE

---

## Blynk Datastream

| Virtual Pin | Description |
|-------------|-------------|
| V0 | LPG |
| V1 | CO |
| V2 | Temperature |
| V3 | Gas Alarm |
| V4 | Fan Status |
| V5 | Valve Status |
| V6 | Packet Number |
| V7 | Receiver Status |
| V8 | Temperature Status |

---

## Receiver Workflow

```
ESP-NOW Receive
        │
        ▼
Decode Packet
        │
        ▼
Serial Monitor
        │
        ▼
Blynk Cloud
        │
        ▼
Alarm Decision
        │
        ▼
Buzzer
```

---

## Buzzer Pattern

| Condition | Pattern |
|-----------|---------|
| Normal | OFF |
| Gas Alarm | Fast Blink |
| Cold Temperature | Continuous ON |
| Communication Lost | Slow Blink |

---

## Communication Status

ONLINE

Packet received normally.

OFFLINE

No packet received within 10 seconds.

---

## Remote Monitoring

The receiver uploads all received data to the Blynk Cloud, enabling real-time monitoring through the Blynk mobile application.
