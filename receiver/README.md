# ESP32 Receiver

![ESP32](https://img.shields.io/badge/ESP32-Receiver-blue)
![ESP-NOW](https://img.shields.io/badge/Protocol-ESP--NOW-green)
![Blynk](https://img.shields.io/badge/Cloud-Blynk-red)

## Overview

The receiver node functions as the monitoring station (Control Room) for the ChickAlert system.

It performs the following tasks:

- Receives sensor data from the ESP32 transmitter using ESP-NOW.
- Displays monitoring data through the Serial Monitor.
- Sends monitoring data to the Blynk Cloud.
- Activates a local buzzer when dangerous conditions occur.
- Detects communication failures between transmitter and receiver.

---

# System Architecture

```
                 ESP-NOW
┌──────────────────────────────┐
│      ESP32 Transmitter       │
│                              │
│ MQ6                          │
│ MQ7                          │
│ DHT22                        │
└──────────────────────────────┘

                │

                ▼

┌──────────────────────────────┐
│       ESP32 Receiver         │
│                              │
│  Serial Monitor              │
│  Active Buzzer               │
│  WiFi                        │
│  Blynk Cloud                 │
└──────────────────────────────┘

                │

                ▼

         Blynk Mobile App
```

---

# Hardware

| Component | GPIO |
|-----------|------|
| Active Buzzer | GPIO25 |

---

# Software Requirements

- Arduino IDE 2.x
- ESP32 Board Package
- Blynk Library

Required libraries:

```
WiFi.h
esp_now.h
esp_wifi.h
BlynkSimpleEsp32.h
```

---

# WiFi Configuration

Edit the following lines before uploading.

```cpp
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
```

Example

```cpp
char ssid[] = "ChickenFarm";
char pass[] = "12345678";
```

The receiver must remain connected to WiFi because it uploads all received data to the Blynk Cloud.

---

# Blynk Configuration

Create a new Blynk Template.

Example

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "ChickAlert"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

These values can be copied directly from the Blynk Console.

---

# ESP-NOW Configuration

The receiver automatically initializes ESP-NOW after connecting to WiFi.

```cpp
WiFi.mode(WIFI_STA);

esp_now_init();

esp_now_register_recv_cb(onReceive);
```

> **Important**
>
> The transmitter and receiver **must use the same WiFi channel**.
>
> Otherwise ESP-NOW packets cannot be received.

---

# Data Structure

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

Every packet contains

- LPG concentration
- CO concentration
- Temperature
- Gas Alarm Status
- Fan Status
- Valve Status
- Packet Number

---

# Blynk Datastream

Create the following Datastreams.

| Virtual Pin | Data Type | Description |
|-------------|-----------|-------------|
| V0 | Double | LPG (ppm) |
| V1 | Double | CO (ppm) |
| V2 | Double | Temperature (°C) |
| V3 | Integer | Gas Alarm |
| V4 | Integer | Fan Status |
| V5 | Integer | Valve Status |
| V6 | Integer | Packet Number |
| V7 | String | Receiver Status |
| V8 | String | Temperature Status |

---

# Recommended Widgets

| Widget | Datastream |
|---------|------------|
| Gauge | V0 |
| Gauge | V1 |
| Gauge | V2 |
| LED | V3 |
| LED | V4 |
| LED | V5 |
| Value Display | V6 |
| Label | V7 |
| Label | V8 |

---

# Receiver Logic

After receiving data, the ESP32 executes

```
Receive Packet

↓

Decode Packet

↓

Update Blynk

↓

Check Alarm

↓

Activate Buzzer
```

---

# Gas Alarm

Condition

$$
GasAlarm = TRUE
$$

Action

- Fast buzzer (250 ms)

---

# Cold Temperature Alarm

Condition

$$
Temperature<25^\circ C
$$

Action

- Continuous buzzer
- Display

```
KANDANG DINGIN
```

---

# Communication Loss

If no packet is received for 10 seconds,

$$
t>10s
$$

the receiver assumes communication failure.

Action

- Slow buzzer (1 second)
- Receiver status becomes

```
OFFLINE
```

---

# Buzzer Priority

Priority order

$$
GasAlarm
>
ColdTemperature
>
CommunicationLost
>
Normal
$$

---

# Communication Status

| Status | Description |
|---------|-------------|
| ONLINE | Packet received normally |
| OFFLINE | No packet received for more than 10 seconds |

---

# Mathematical Representation

Gas Alarm

$$
Alarm=
\begin{cases}
1,& LPG \ge Threshold\\
0,& \text{otherwise}
\end{cases}
$$

Temperature Alert

$$
Cold=
\begin{cases}
1,& T<25^\circ C\\
0,& \text{otherwise}
\end{cases}
$$

Communication Status

$$
Status=
\begin{cases}
ONLINE,& t\le10s\\
OFFLINE,& t>10s
\end{cases}
$$

---

# Upload Procedure

1. Install ESP32 Board Package.
2. Install Blynk Library.
3. Open `receiver.ino`.
4. Configure WiFi SSID and Password.
5. Configure Blynk Template ID and Auth Token.
6. Select the correct COM Port.
7. Upload the sketch.
8. Open Serial Monitor (115200 baud).

---

# Expected Serial Output

```
====================================
ESP32 RECEIVER
====================================

Receiver MAC :
88:57:21:2D:34:84

Waiting data...

====================================
DATA RECEIVED
====================================

Packet : 35
LPG : 120 ppm
CO : 18 ppm
Temperature : 28.5 C

Gas Alarm : NO
Fan : OFF
Valve : OPEN
```

---

# Troubleshooting

## ESP-NOW does not receive data

- Check receiver MAC address.
- Ensure both ESP32 boards use the same WiFi channel.
- Verify transmitter MAC configuration.
- Make sure ESP-NOW is initialized successfully.

---

## Blynk not updating

- Verify WiFi connection.
- Verify Auth Token.
- Confirm Virtual Pin assignments.
- Check internet connection.

---

## Receiver always OFFLINE

Possible causes

- Transmitter not powered.
- Incorrect receiver MAC.
- Different WiFi channels.
- ESP-NOW initialization failed.

---

# Author

**Harry Prasetyo Rosianto**

CCIT Faculty of Engineering

Universitas Indonesia
