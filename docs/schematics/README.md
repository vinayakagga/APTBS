# Schematics

- `system-block.svg` — high-level control architecture.
- `wiring.svg` — ESP32, PCA9685, external servo supply, and five servo connections.

The schematics reflect the connections supported by the current firmware: ESP32 I²C on GPIO 21/22, PCA9685 at `0x40`, 50 Hz PWM, and servo channels CH0–CH4.
