# 5-DOF Robot Arm Control — ESP32 + PCA9685 + WebSocket

A standalone control stack for a 5-DOF servo robotic arm.

This repository intentionally covers **only the arm-control subsystem**:

**Browser Dashboard → Wi-Fi/WebSocket → ESP32 → I²C → PCA9685 → 5 Servos**

It does not include OCR, camera, speech/audio, page-turning logic, or other application-level subsystems.

## Features

- Browser-based 5-joint control dashboard
- ESP32 WebSocket server on port `81`
- JSON joint commands
- `HOME` and `STOP` commands
- PCA9685 16-channel PWM servo driver
- 5 active servo channels: CH0–CH4
- 50 Hz servo PWM
- Per-joint software angle limits
- Per-joint movement speed
- Smooth target-to-current servo motion

## Hardware

| Component | Quantity | Connection / role |
|---|---:|---|
| ESP32 development board | 1 | Wi-Fi + WebSocket server + I²C controller |
| PCA9685 16-channel servo driver | 1 | Generates servo PWM |
| Servo motors | 5 | PCA9685 CH0–CH4 |
| External servo power supply | 1 | Powers the servos through PCA9685 V+ |
| Computer / phone on same LAN | 1 | Hosts/opens `dashboard/dash.html` |

### Important power note

**Do not power the five servos from the ESP32 3.3 V rail.** Use a suitable external 5–6 V servo supply. Connect the external supply ground, PCA9685 ground, and ESP32 ground together.

The exact current rating and voltage must be selected for the actual servos and their simultaneous load.

## Pinout

### ESP32 → PCA9685

| ESP32 | PCA9685 | Purpose |
|---|---|---|
| GPIO 21 | SDA | I²C data |
| GPIO 22 | SCL | I²C clock |
| GND | GND | Common ground |
| 3.3 V | VCC | PCA9685 logic supply |

The firmware initializes I²C with `Wire.begin(21, 22)` and the PCA9685 at address `0x40`.

### PCA9685 → Servos

| PCA9685 channel | Servo |
|---|---|
| CH0 | Joint 0 |
| CH1 | Joint 1 |
| CH2 | Joint 2 |
| CH3 | Joint 3 |
| CH4 | Joint 4 |

Connect each servo's signal wire to its PCA9685 channel, its power wire to the PCA9685 servo-power rail, and its ground wire to the PCA9685 ground.

## Software architecture

```text
+-----------------------+
|  Browser              |
|  dashboard/dash.html  |
+-----------+-----------+
            |
            | WebSocket / TCP :81
            | JSON
            v
+-----------------------+
| ESP32                 |
| WebSocket server      |
| Joint limits          |
| Smooth motion         |
+-----------+-----------+
            |
            | I²C
            | SDA=GPIO21
            | SCL=GPIO22
            v
+-----------------------+
| PCA9685 @ 0x40       |
| PWM = 50 Hz          |
+--+----+----+----+----+
   |    |    |    |    |
  CH0  CH1  CH2  CH3  CH4
   |    |    |    |    |
   v    v    v    v    v
  J0   J1   J2   J3   J4
```

## WebSocket protocol

### Joint movement

```json
{"type":"joint","ch":2,"angle":110}
```

- `type`: `joint`
- `ch`: integer `0`–`4`
- `angle`: requested angle in degrees
- Firmware clamps the requested angle to that channel's configured limits.

### Home

```json
{"type":"home"}
```

Current home targets:

```text
CH0 = 90°
CH1 = 130°
CH2 = 130°
CH3 = 90°
CH4 = 90°
```

### Stop

```json
{"type":"stop"}
```

`STOP` changes each target to the arm's current software position, causing the smooth-motion loop to stop at its current position.

## Current joint limits and speeds

| Channel | Initial/Home | Min | Max | Speed |
|---|---:|---:|---:|---:|
| CH0 | 90° | 10° | 170° | 15°/s |
| CH1 | 130° | 30° | 170° | 5°/s |
| CH2 | 130° | 30° | 170° | 5°/s |
| CH3 | 90° | 10° | 170° | 15°/s |
| CH4 | 90° | 10° | 170° | 15°/s |

These are **software limits from the current firmware**, not guaranteed mechanical-safe limits. Validate the real arm's physical travel before operating under load.

## Setup

### 1. Arduino IDE libraries

Install:

- `WebSockets` / `WebSocketsServer`
- `ArduinoJson`
- `Adafruit PWM Servo Driver Library`

The firmware also uses the ESP32 `WiFi` and `Wire` libraries.

### 2. Configure Wi-Fi

Copy:

```text
firmware/config.example.h
```

to:

```text
firmware/config.h
```

and enter your local Wi-Fi credentials. `config.h` is ignored by Git so credentials are not committed.

### 3. Upload firmware

Open `firmware/handcntr.ino`, select your ESP32 board, compile and upload.

Open Serial Monitor at **115200 baud**. The firmware prints the ESP32 IP address after connecting to Wi-Fi.

### 4. Set dashboard IP

Open `dashboard/dash.html` and set:

```javascript
let ESP32_IP = "YOUR_ESP32_IP";
```

to the IP printed by the ESP32.

### 5. Open dashboard

Open `dashboard/dash.html` in a browser on the same LAN as the ESP32.

The dashboard connects to:

```text
ws://ESP32_IP:81
```

## Safety / commissioning checklist

1. Keep the arm unloaded for first power-up.
2. Verify the servo power supply voltage before connecting servos.
3. Confirm common ground between ESP32 and PCA9685.
4. Test one servo at a time where practical.
5. Verify the configured software limits against the physical mechanism.
6. Keep clear of the arm while testing.
7. Use `STOP` if the arm moves unexpectedly.
8. A software `STOP` is not a substitute for a physical emergency-stop circuit when working with a high-torque arm.

## Repository scope

This repo documents the current **arm-control layer only**. Higher-level functions such as page turning, camera capture, OCR/HTR, speech, and knowledge-core ingestion are intentionally outside this repository.
