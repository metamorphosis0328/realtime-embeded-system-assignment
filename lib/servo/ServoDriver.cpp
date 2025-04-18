#include "servo/ServoDriver.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

// Constructor: Initializes I2C communication with PCA9685
ServoDriver::ServoDriver(int i2cBus, int address)
    : i2cAddr(address)
{
    std::string dev = "/dev/i2c-" + std::to_string(i2cBus);
    i2cFd = open(dev.c_str(), O_RDWR);
    if (i2cFd < 0)
    {
        perror("open");
        throw std::runtime_error("Failed to open I2C device");
    }

    if (ioctl(i2cFd, I2C_SLAVE, i2cAddr) < 0)
    {
        perror("ioctl");
        throw std::runtime_error("Failed to set I2C address");
    }

    std::cout << "[ServoDriver] Connected to PCA9685 at 0x" << std::hex << address << std::dec << std::endl;

    // Initialize PCA9685: set to sleep mode, set prescale, wake up, enable auto-increment
    writeRegister(0x00, 0x00); // MODE1 normal
    writeRegister(0x00, 0x10); // Sleep mode
    writeRegister(0xFE, 121);  // Prescale for 50Hz PWM
    writeRegister(0x00, 0x00); // Wake up
    usleep(5000);
    writeRegister(0x00, 0xA1); // Enable auto-increment
}

// Destructor: Close I2C file descriptor
ServoDriver::~ServoDriver()
{
    if (i2cFd > 0)
        close(i2cFd);
}

// Write a single register to the PCA9685
void ServoDriver::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = {reg, value};
    if (write(i2cFd, buffer, 2) != 2)
    {
        std::cerr << "[I2C] Failed to write reg 0x" << std::hex << (int)reg << std::endl;
    }
}

// Set PWM ON and OFF tick counts for a specific channel
void ServoDriver::setPWM(int channel, int onTick, int offTick)
{
    uint8_t reg = 0x06 + 4 * channel; // Base register for LEDn
    uint8_t buffer[5] = {
        reg,
        static_cast<uint8_t>(onTick & 0xFF),
        static_cast<uint8_t>((onTick >> 8) & 0xFF),
        static_cast<uint8_t>(offTick & 0xFF),
        static_cast<uint8_t>((offTick >> 8) & 0xFF)};
    if (write(i2cFd, buffer, 5) != 5)
    {
        std::cerr << "[I2C] Failed to write PWM data for channel " << channel << std::endl;
    }
}

// Convert pulse width in microseconds to tick and apply it to the channel
void ServoDriver::setServoPulseUs(int channel, int pulseUs)
{
    // Each tick ≈ 4.88us (at 50Hz: 20000us / 4096)
    int tick = static_cast<int>(pulseUs / 4.88);
    std::cout << "[Servo] pulse=" << pulseUs << "us => ticks=" << tick << std::endl;
    setPWM(channel, 0, tick);
}

// Set servo angle according to provided ServoConfig
void ServoDriver::setServoAngle(const ServoConfig &config, float angleDeg)
{
    // Clamp angle within config range
    angleDeg = std::clamp(angleDeg, config.minAngle, config.maxAngle);

    // Calculate normalized ratio of the angle
    float ratio = (angleDeg - config.minAngle) / (config.maxAngle - config.minAngle);

    // Map angle to pulse width
    int pulseUs = config.minAnglePulseUs + static_cast<int>(ratio * (config.maxAnglePulseUs - config.minAnglePulseUs));

    // Output angle information
    std::cout << "[Arm] angle=" << angleDeg << "°" << std::endl;

    setServoPulseUs(config.channel, pulseUs);
}

// Release all servos
void ServoDriver::releaseServos()
{
    setPWM(BASE_SERVO.channel, 0, 0);
    setPWM(SHOULDER_SERVO.channel, 0, 0);
    setPWM(ELBOW_SERVO.channel, 0, 0);

    std::cout << "[Servo] All servos released." << std::endl;
}

// Release a specific servo
void ServoDriver::releaseServos(const ServoConfig &cfg)
{
    setPWM(cfg.channel, 0, 0);
    std::cout << "[Servo] Servo on channel " << cfg.channel << " released." << std::endl;
}

// Reset all servos to center
void ServoDriver::resetServos()
{
    setServoAngle(BASE_SERVO, 0.0f);
    setServoAngle(SHOULDER_SERVO, 0.0f);
    setServoAngle(ELBOW_SERVO, 0.0f);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    releaseServos(); // Release to prevent jitter

    std::cout << "[Servo] All servos reset to 0°." << std::endl;
}

// Reset a specific servo to center
void ServoDriver::resetServos(const ServoConfig &config)
{
    setServoAngle(config, 0.0f);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    releaseServos(config); // Release to prevent jitter

    std::cout << "[Servo] Servo on channel " << config.channel << " reset to 0°." << std::endl;
}