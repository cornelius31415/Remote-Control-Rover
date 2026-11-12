# SunFounder Mars Rover — Wireless Joystick Control with ESP-NOW and UART

## Overview

This project is based on the **SunFounder Mars Rover (GalaxyRVR)**, which uses an **Arduino Uno R3** and the GalaxyRVR motor shield. I added an **ESP32 receiver** to the rover so it can be controlled wirelessly with a joystick.

A second ESP32 reads the joystick position and sends the control values to the rover over **ESP-NOW**. The ESP32 mounted on the rover forwards the received values to the Arduino over **UART**. The Arduino then controls the motors using the `SoftPWM` library.

The result is a rover that can drive forward, backward, and turn left or right using a wireless joystick.

## System Architecture

```text
Joystick
  | X axis, Y axis, push button
  v
ESP32 (transmitter)
  |
  | ESP-NOW (wireless)
  v
ESP32 (receiver on rover)
  |
  | UART, 9600 baud
  v
Arduino Uno R3
  |
  | SoftPWM, motor pins D2–D5
  v
SunFounder GalaxyRVR motor shield
  |
  v
Left and right rover motors
```

## Hardware

- SunFounder Mars Rover / GalaxyRVR with Arduino Uno R3 and motor shield
- Two ESP32 development boards: one for the joystick, one mounted on the rover
- Two-axis analog joystick with push button
- Jumper wires and suitable power supplies

## Communication

### 1. Joystick to ESP32 Transmitter

The transmitting ESP32 reads both analog joystick axes and the push button.

| Joystick pin | ESP32 transmitter pin |
|---|---|
| VRx | GPIO34 |
| VRy | GPIO35 |
| SW | GPIO32 |
| VCC | 3V3 |
| GND | GND |

### 2. ESP32 Transmitter to ESP32 Receiver

The joystick data is sent wirelessly using **ESP-NOW**, which allows direct communication between compatible ESP devices without a conventional Wi-Fi router.

### 3. ESP32 Receiver to Arduino Uno

The receiver forwards joystick data to the Arduino using **UART at 9600 baud**.

| ESP32 receiver | Arduino Uno |
|---|---|
| GPIO17 (TX) | A0 (SoftwareSerial RX) |
| GND | GND |

The Arduino uses `SoftwareSerial(A0, A1)`. A1 is configured as the software serial TX pin but is not used for this one-way connection. The ESP32 and Arduino must share a common ground.

The UART message format is:

```text
X,Y,BUTTON\n
```

Example:

```text
2048,3500,1
```

`X` and `Y` are joystick ADC readings, typically ranging from 0 to 4095. `BUTTON` indicates the push-button state.

## Motor Control

The Arduino controls the GalaxyRVR motor shield through `SoftPWM` using the following pins:

| Arduino pin | Function |
|---|---|
| D2 (`in1`) | Left motor group, direction 1 |
| D3 (`in2`) | Left motor group, direction 2 |
| D4 (`in3`) | Right motor group, direction 1 |
| D5 (`in4`) | Right motor group, direction 2 |

Both joystick axes are used:

- **Y axis:** Forward and backward movement
- **X axis:** Left and right steering
- **Combined X/Y input:** Turning while moving, using different speeds for the left and right motor groups
- **Joystick button:** Stop

Steering uses **differential drive**: throttle and steering inputs are combined into separate target speeds for the left and right motor groups.

## Safety and Connection Loss

The Arduino program includes a **300 ms UART timeout**: if no valid UART message is received during that period, the motors stop.

**Important:** To handle an ESP-NOW radio disconnect safely, the receiver ESP32 must also detect missing wireless updates and send a stop command (or stop forwarding stale commands). The Arduino UART timeout alone does not detect a wireless disconnect if the receiver keeps retransmitting old values.

## Result

The **SunFounder Mars Rover can now be controlled wirelessly with a joystick**. ESP-NOW handles communication between the two ESP32 boards, UART connects the rover's ESP32 receiver to the Arduino Uno, and the Arduino continues to drive the motors through the GalaxyRVR shield.

## Possible Future Improvements

- Send sensor readings and battery status back to the controller
- Add wheel encoders and closed-loop speed control
- Improve packet validation and radio-loss failsafe behavior
- Implement autonomous navigation using distance sensors or a camera
- Integrate a robotic arm
