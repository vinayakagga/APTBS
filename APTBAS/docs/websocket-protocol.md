# WebSocket Protocol

Server: ESP32

Port: `81`

Transport: WebSocket over the local Wi-Fi network

## Connect

```text
ws://<ESP32-IP>:81
```

## Commands

### Joint

```json
{"type":"joint","ch":0,"angle":90}
```

The firmware accepts channels 0–4. Requested angles are constrained to the configured per-channel minimum and maximum.

### Home

```json
{"type":"home"}
```

Sets the five target angles to:

```text
[90, 130, 130, 90, 90]
```

### Stop

```json
{"type":"stop"}
```

Sets each target angle to its current software position.

## Motion behavior

The firmware maintains `currentAngle[]` and `targetAngle[]`. Every loop iteration, the current position is moved toward the target at the configured speed for that joint.
