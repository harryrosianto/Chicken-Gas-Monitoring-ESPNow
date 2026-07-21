# ESP32 Transmitter

## Overview

The transmitter node is responsible for acquiring environmental data inside the chicken coop, processing the sensor readings, controlling the actuators, and sending the processed data wirelessly to the receiver using the ESP-NOW protocol.

---

# Hardware Specification

| Component | GPIO |
|----------|------|
| MQ6 | GPIO32 |
| MQ7 | GPIO33 |
| DHT22 | GPIO4 |
| Fan Relay | GPIO18 |
| Valve Relay | GPIO19 |
| LCD I2C SDA | GPIO21 |
| LCD I2C SCL | GPIO22 |

---

# Sensor Processing

## MQ6 (LPG)

The MQ6 sensor was calibrated against a commercial gas detector.

Calibration points:

| ADC | LPG |
|----:|----:|
|1968|0 ppm|
|3148|884 ppm|

The slope is

$$
m=\frac{884-0}{3148-1968}
$$

$$
m=0.749
$$

Therefore,

$$
LPG_{ppm}=0.749(ADC-1968)
$$

or

$$
LPG_{ppm}=\frac{884}{1180}(ADC-1968)
$$

Arduino implementation

```cpp
float mq6ToPPM(int adc)
{
    float ppm = (adc - 1968) * 884.0 / 1180.0;

    if(ppm < 0)
        ppm = 0;

    return ppm;
}
```

---

## MQ7 (CO)

The MQ7 sensor response is nonlinear. Therefore, piecewise linear interpolation is used.

Calibration dataset

| ADC | CO (ppm) |
|----:|---------:|
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

For every ADC value located between two calibration points

$$
(ADC_1,C_1)
$$

and

$$
(ADC_2,C_2)
$$

the CO concentration is estimated using linear interpolation

$$
C=C_1+\frac{(ADC-ADC_1)(C_2-C_1)}{ADC_2-ADC_1}
$$

where

- \(C\) = estimated CO concentration (ppm)
- \(ADC\) = measured ADC value
- \(ADC_1,ADC_2\) = surrounding calibration ADC values
- \(C_1,C_2\) = calibrated CO concentrations

---

## Temperature

Temperature is measured using the DHT22 sensor.

If calibration is required,

$$
T=T_{raw}+Offset
$$

Example

$$
T=T_{raw}+0.5
$$

---

# Alarm Logic

The gas alarm is activated whenever

$$
LPG \ge LPG_{limit}
$$

where

$$
LPG_{limit}=884\ ppm
$$

The relay is reset when

$$
LPG \le LPG_{reset}
$$

This hysteresis prevents relay oscillation.

---

# Data Packet

The transmitted packet consists of

$$
Packet=
\{
LPG,
CO,
Temperature,
GasAlarm,
FanStatus,
ValveStatus,
PacketNumber
\}
$$

---

# Communication

Protocol

- ESP-NOW

Transmission interval

$$
t=2\ seconds
$$

---

# Workflow

```text
MQ6
MQ7
DHT22
   │
   ▼
Read Sensors
   │
   ▼
Calibration
   │
   ▼
Relay Control
   │
   ▼
ESP-NOW Transmission
```
