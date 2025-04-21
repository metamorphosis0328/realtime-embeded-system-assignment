// Adapted from Adafruit's PCA9685 PWM Servo Driver Library
// https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
//
// Original code Copyright (c) 2012, Adafruit Industries
// Licensed under the BSD License
// See LICENSE-THIRD-PARTY.txt for full license text.
//
// Significant modifications for use in the ENG5220 Gomoku AI Robot System (2025):
// - Ported from Arduino to Raspberry Pi (Linux-based I2C interface using /dev/i2c-*)
// - Replaced Arduino-specific APIs (Wire, delay, etc.) with standard Linux C++
// - Redesigned as a C++ class with better abstraction and integration for servo control

#include "PCA9685Driver.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cmath>
#include <iostream>

// PCA9685 register addresses
#define MODE1 0x00
#define PRESCALE 0xFE
#define LED0_ON_L 0x06

/**
 * @brief Constructor for PCA9685Driver.
 *
 * Initializes the I2C address and sets default PWM frequency to 50 Hz.
 * which applies to all peripherals including servos, the pump, and the electromagnet.
 *
 * @param i2cAddress The I2C address of the PCA9685.
 */
PCA9685Driver::PCA9685Driver(uint8_t i2cAddress)
    : address(i2cAddress), i2c_fd(-1), pwmFrequency(50.0f) {}

/**
 * @brief Destructor for PCA9685Driver.
 *
 * Closes the I2C file descriptor if open.
 */
PCA9685Driver::~PCA9685Driver()
{
    if (i2c_fd >= 0)
        close(i2c_fd);
}

/**
 * @brief Initializes the I2C interface and sets default configuration.
 *
 * Opens the I2C device and resets the PCA9685 to normal mode.
 *
 * @return true if successful.
 * @return false if failed.
 */
bool PCA9685Driver::begin()
{
    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0)
        return false;

    if (ioctl(i2c_fd, I2C_SLAVE, address) < 0)
        return false;

    write8(MODE1, 0x00); // Set to normal mode
    setPWMFreq(pwmFrequency);
    return true;
}

/**
 * @brief Sets the PWM frequency for the entire PCA9685.
 *
 * The prescale value is calculated based on the desired frequency.
 * The chip must be put into sleep mode before updating the prescaler.
 *
 * @param freqHz Desired frequency in Hz.
 */
void PCA9685Driver::setPWMFreq(float freqHz)
{
    pwmFrequency = freqHz;

    float prescaleval = 25000000.0; // 25MHz internal clock
    prescaleval /= 4096.0;          // 12-bit resolution
    prescaleval /= freqHz;
    prescaleval -= 1.0;

    uint8_t prescale = static_cast<uint8_t>(std::floor(prescaleval + 0.5));

    uint8_t oldmode = read8(MODE1);
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // Enter sleep mode
    write8(MODE1, newmode);                    // Go to sleep
    write8(PRESCALE, prescale);                // Set prescale
    write8(MODE1, oldmode);                    // Wake up
    usleep(5000);
    write8(MODE1, oldmode | 0x80); // Restart
}

/**
 * @brief Sets a pulse width (in microseconds) for a specific PWM channel.
 *
 * Converts the microsecond value to a duty cycle based on the set frequency.
 *
 * @param channel PWM output channel (0–15).
 * @param microseconds Pulse width in microseconds.
 */
void PCA9685Driver::setPulseWidth(uint8_t channel, float microseconds)
{
    float period_us = 1000000.0f / pwmFrequency;
    uint16_t pwm_val = static_cast<uint16_t>(4096 * microseconds / period_us);

    if (pwm_val > 4095)
        pwm_val = 4095;

    setPWM(channel, 0, pwm_val);
}

/**
 * @brief Sets the ON and OFF tick values for a specific channel.
 *
 * This provides full control over PWM timing.
 *
 * @param channel PWM output channel (0–15).
 * @param on Tick when signal should turn ON (0–4095).
 * @param off Tick when signal should turn OFF (0–4095).
 */
void PCA9685Driver::setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    uint8_t reg = LED0_ON_L + 4 * channel;
    write8(reg, on & 0xFF);
    write8(reg + 1, on >> 8);
    write8(reg + 2, off & 0xFF);
    write8(reg + 3, off >> 8);
}

/**
 * @brief Writes a single byte to a register.
 *
 * @param reg Register address.
 * @param value Byte to write.
 * @return true if succeeds.
 * @return false if fails.
 */
bool PCA9685Driver::write8(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = {reg, value};
    return write(i2c_fd, buffer, 2) == 2;
}

/**
 * @brief Reads a single byte from a register.
 *
 * @param reg Register address.
 * @return The byte read from the register.
 */
uint8_t PCA9685Driver::read8(uint8_t reg)
{
    write(i2c_fd, &reg, 1);
    uint8_t value = 0;
    read(i2c_fd, &value, 1);
    return value;
}
