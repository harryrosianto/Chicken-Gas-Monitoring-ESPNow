# Decision Logic

## Gas Alarm

$$
GasAlarm=True
$$

↓

Fast buzzer (250 ms)

---

## Low Temperature

$$
Temperature<25^\circ C
$$

↓

Continuous buzzer

---

## Communication Loss

$$
t_{last\ packet}>10\ seconds
$$

↓

Slow buzzer

---

## Buzzer Priority

$$
GasAlarm
>
ColdAlert
>
CommunicationLost
>
Normal
$$

---

## Communication Status

$$
Status=
\begin{cases}
ONLINE,& Packet\ received\\
OFFLINE,& Packet\ timeout>10s
\end{cases}
$$
