# Gomoku AI Robot System

This project is a real-time embedded system developed as part of the **ENG5220 Real-Time Embedded Programming** coursework. It features an autonomous Gomoku-playing robot that integrates computer vision, AI-based decision-making, and robotic arm control. The system utilizes an **event-driven callback mechanism** for the vision module, a **non-blocking state machine** for the arm control logic, and a **multi-threaded architecture** to enable concurrent operation of all subsystems. And it's designed to detect human moves via a camera, compute the best counter-move using a Minimax-based AI engine, and place pieces physically using a servo-driven robotic arm.

---

## Acknowledgements

- The PCA9685 driver in this project is **adapted from** [Adafruit's PCA9685 PWM Servo Driver Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library), originally released under the BSD License. Significant modifications were made to fit the needs of this real-time embedded coursework project.

- Significant modifications for use in the ENG5220 Gomoku AI Robot System (2025):
   - Ported from Arduino to Raspberry Pi (Linux-based I2C interface using /dev/i2c-*)
   - Replaced Arduino-specific APIs (Wire, delay, etc.) with standard Linux C++
   - Redesigned as a C++ class with better abstraction and integration for servo control

---

## Features

- **AI Module**: Implements a Minimax-based algorithm to determine the best move based on real-time board updates.

- **Vision System (Event-Driven)**: Continuously captures board state using OpenCV and uses callbacks to notify other components when a new piece is detected.

- **Robotic Arm (Non-blocking state machine)**: Places pieces using servo motors and a suction-based gripping mechanism. Uses bilinear interpolation to map coordinates to servo angles.

- **Real-Time Coordination (Callbacks & Multithreading)**: Uses multithreading to isolate servo control from other subsystems, using a non-blocking, time-driven state machine for precise movement scheduling. Vision detection is decoupled via a callback mechanism, allowing asynchronous event handling and smooth coordination between vision, AI, and arm modules.

---

## Architecture

```
.
├── include
│   ├── app
│   │   ├── algorithm
│   │   ├── arm
│   │   ├── camera
│   │   └── coordinator
│   └── driver
├── src
│   ├── app
│   │   ├── algorithm
│   │   ├── arm
│   │   ├── camera
│   │   └── coordinator
│   ├── driver
│   └── main.cpp
```

---

## How It Works

1. A camera continuously captures the board image stream.
2. The `GomokuVision` module processes frames and uses computer vision (Hough Transform, intensity detection) to locate and classify new pieces.
3. When a valid new piece is confirmed over several frames, it triggers a callback (`onNewPieceDetected()`), alerting the system.
4. The `GomokuCoordinator`, implementing the callback interface, checks whose turn it is. If it's the AI's turn, it queries the `GomokuAI` module, which uses Minimax to select the best move.
5. The `ArmController` executes the move:
   - Interpolates servo angles using bilinear interpolation
   - Controls the arm through a non-blocking, time-driven state machine to achieve smooth and responsive motion
6. The AI’s move is updated on the board, and the cycle repeats until a winner is found.

---

## Requirements

- C++17 or later
- OpenCV

### Install OpenCV on Linux

```bash
sudo apt update
sudo apt install libopencv-dev
```

### Enable I2C on Raspberry Pi

```bash
sudo raspi-config
```

Navigate to `Interfacing Options` → `I2C` → `Enable`

Exit and reboot your Raspberry Pi

Install I2C tools (for testing and debugging):

```bash
sudo apt update
sudo apt install -y i2c-tools
```

Check connected I2C devices

```bash
i2cdetect -y 1
```

You should see a device listed at address 0x40 (default for PCA9685).

---

## Hardware

- Raspberry Pi
- Robotic arm with 3 servos
- Vacuum pump and electromagnet for gripping
- PCA9685 PWM driver module
- Camera

---

## Wiring Guide

### 1. PCA9685 - Raspberry Pi (I2C)

| PCA9685 Pin | Connects To (Raspberry Pi GPIO) | Description   |
|-------------|-------------------------------|-----------------|
| VCC         | 5V (Pin 3)                  | Power for logic |
| GND         | GND (Pin 6)                   | Ground          |
| SDA         | GPIO2 / SDA (Pin 4)           | I2C data        |
| SCL         | GPIO3 / SCL (Pin 5)           | I2C clock       |
| V+          | 5V (External 5V)              | Power for servos (separate from logic) |

### 2. PCA9685 - Actuators (Servo / Pump / Electromagnet)

| PCA9685 Channel | Device         | Description                       |
|------------------|----------------|----------------------------------|
| Channel 0        | Elbow Servo    | Controls the elbow joint         |
| Channel 1        | Base Servo     | Controls rotation base           |
| Channel 2        | Shoulder Servo | Controls the shoulder joint      |
| Channel 14       | Electromagnet  | Turns magnet ON/OFF              |
| Channel 15       | Vacuum Pump    | Turns suction ON/OFF             |

**Important**: Make sure all power sources share a **common ground (GND)**

---

## Build Instructions

```bash
cmake .
make
```

---

## Notes

- The AI assumes the human plays black and the robot plays white.
- Vision logic uses a combination of Hough Circles and grayscale intensity to detect black and white pieces. Make sure the lighting is sufficient and there are no shadows on the board.
- The vision detection mechanism requires the **entire board** to be visible within the camera frame, especially the **edges and corners**. Incomplete visibility may result in incorrect or failed coordinate mapping, as the system relies on full board geometry for perspective transformation.
- Arm movement angles are calculated using bilinear interpolation from a 3x3 manually calibrated grid.

---

## Limitations & Future Work

- The system is a prototype and not yet fully robust in terms of physical placement accuracy, and the **gripping functionality** is currently under development.
- Vision system currently uses simple grayscale thresholding; color detection could be improved with better lighting adaptation or ML-based classification.
- AI depth is fixed and does not adapt to difficulty level or performance constraints.
- The robotic arm relies on manually calibrated angles, and real-time kinematics is not yet implemented.
- No GUI or user interface for game state or AI settings.
- The current system architecture lacks sufficient decoupling between modules, which affects maintainability and scalability.

---

## Demo video

- **Douyin (TikTok China version):** [https://v.douyin.com/ni1dFG2RaWw/](https://v.douyin.com/ni1dFG2RaWw/)

(The **gripping functionality** is not functional for now, so white pieces are placed manually. However, the robotic arm correctly responds to AI moves and points to the intended placement location.)
 ![demo](https://github.com/user-attachments/assets/3c09957a-0d09-426d-88bf-5494eae97663)

---

> ⚠️ This is an early-stage prototype. Feedback and contributions are welcome!
