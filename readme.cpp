// Realtime Embedded System Assignment
// C++ real?time embedded system for a Gomoku robot

//Features 
PCA9685 PWM Driver
I2C interface to a 16-channel PWM controller for servos, pump, and electromagnet.  
Servo Control
Calibrated angle-to-pulse mapping with clamping to mechanical limits.  
Arm Controller
High-level API (`ArmController`) to initialize, move, and reset a 3?DOF robotic arm (base, shoulder, elbow).  
Camera Module 
OpenCV-based Gomoku board corner detection, perspective warp, and stone?presence extraction.  
Test Suites
Console?based tests for the PCA9685 driver, `Servo`, and `ArmController` modules.
 
 
 
// Dependencies

Compiler: g++ (or Clang) with C++20 support  
Build system: CMake ¡Ý 3.10  
Libraries:  
OpenCV ¡Ý 4.0  
Linux I2C (`i2c-dev`)  
Hardware:  
PCA9685 PWM controller (default I2C address 0x40)  
Servos, pump, electromagnet wired to PCA9685 channels  

// Installation

//On Ubuntu/Debian:
bash
sudo apt update
sudo apt install -y build-essential cmake libopencv-dev i2c-tools




//Enable I2C in your kernel
//sudo modprobe i2c-dev

//Builting 
git clone https://github.com/metamorphosis0328/realtime-embeded-system-assignment.git
cd realtime-embeded-system-assignment
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make

//Usage
//# From build directory
./gomoku_robot
Follow the prompts:
0 ¡ª Free control mode (enter <servo_id> <angle>)
1 ¡ª Single?servo adjustment mode
2 ¡ª Automatic test sequence


//Camera Demo
//g++ -std=c++17 src/app/camera/GomokuCam.cpp -o gomoku_cam \
    `pkg-config --cflags --libs opencv4`
./gomoku_cam



//Running Tests
//
./test_pca9685
./test_servo
./test_arm


//License
Feel free to adjust any paths, camera indices, or calibration values to match your hardware setup.


