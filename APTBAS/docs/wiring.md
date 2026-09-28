# Wiring Reference

## ESP32 to PCA9685

```text
ESP32                         PCA9685
--------------------------------------
GPIO 21  -------------------- SDA
GPIO 22  -------------------- SCL
3.3V     -------------------- VCC
GND      -------------------- GND

External 5–6 V supply ------- V+
Supply GND ------------------ GND
```

The external supply powers the servo rail (`V+`). The ESP32's 3.3 V output is for logic/VCC, not for powering the servo motors.

## Servo connections

```text
PCA9685 CH0  -> Joint 0 servo
PCA9685 CH1  -> Joint 1 servo
PCA9685 CH2  -> Joint 2 servo
PCA9685 CH3  -> Joint 3 servo
PCA9685 CH4  -> Joint 4 servo
```

For each servo:

- Signal → corresponding PCA9685 channel signal
- V+ → external servo supply rail
- GND → common ground

Check the actual servo connector/wire color convention from the servo manufacturer before connecting power.
