# Gomoku AI Robot System

This project is a real-time embedded system developed as part of the **ENG5220 Real-Time Embedded Programming** coursework. It features an autonomous Gomoku-playing robot that integrates computer vision, AI-based decision-making, and robotic arm control. The system utilizes **event-driven callbacks**, **blocking I/O**, and **multi-threaded** design to detect human moves via a camera, compute the best counter-move using a Minimax-based AI engine, and place pieces physically using a servo-driven robotic arm.

---

## Features

- **AI Module**: Implements a Minimax-based algorithm to determine the best move based on real-time board updates.

- **Vision System (Event-Driven)**: Continuously captures board state using OpenCV and uses callbacks to notify other components when a new piece is detected.

- **Robotic Arm**: Places pieces using servo motors and a suction-based gripping mechanism. Uses bilinear interpolation to map coordinates to servo angles.

- **Real-Time Coordination (with Callbacks & Blocking I/O)**: Uses blocking I/O for efficient thread management in servo control. Also employs a callback registration mechanism to decouple vision detection from AI and control logic, enabling a responsive and modular design.

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
   - Controls the arm via blocking I/O to ensure smooth, real-time motion
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

---

## Hardware

- Raspberry Pi
- Robotic arm with 3 servos
- Vacuum pump and electromagnet for gripping
- PCA9685 PWM driver module
- Camera

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
- Arm movement angles are calculated using bilinear interpolation from a 3x3 manually calibrated grid.

---

## Limitations & Future Work

- The system is a prototype and not yet fully robust in terms of physical placement accuracy, and some core functionalities are still incomplete.
- Vision system currently uses simple grayscale thresholding; color detection could be improved with better lighting adaptation or ML-based classification.
- AI depth is fixed and does not adapt to difficulty level or performance constraints.
- The robotic arm relies on manually calibrated angles, and real-time kinematics is not yet implemented.
- No GUI or user interface for game state or AI settings.
- The current system architecture lacks sufficient decoupling between modules, which affects maintainability and scalability.

---

> ⚠️ This is an early-stage prototype. Feedback and contributions are welcome!
