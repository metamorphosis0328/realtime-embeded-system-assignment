#include "servo/ServoDriver.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

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
    angleDeg = std::clamp(angleDeg, config.minAngle, config.maxAngle);

    // Calculate normalized ratio of the angle
    float ratio = (angleDeg - config.minAngle) / (config.maxAngle - config.minAngle);

    // Map angle to pulse width
    int pulseUs = config.minAnglePulseUs + static_cast<int>(ratio * (config.maxAnglePulseUs - config.minAnglePulseUs));

    setServoPulseUs(config.channel, pulseUs);
}

// Disable PWM output on all 16 channels (used to stop all servos)
void ServoDriver::releaseAllServos()
{
    for (int channel = 0; channel < 16; ++channel)
    {
        setPWM(channel, 0, 0);
    }
    std::cout << "[ServoDriver] All servos released." << std::endl;
}
