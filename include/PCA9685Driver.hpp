#ifndef PCA9685DRIVER_HPP
#define PCA9685DRIVER_HPP

#include <cstdint>

/**
 * @brief Driver class for the PCA9865 16-channel PWM controller via I2C.
 *
 * This class allows initialization and control of PCA9865 to generate PWM signals.
 * In this project, servos, the pump, and the electromagnet can all be controlled by PWM signals.
 */
class PCA9685Driver
{
public:
    /**
     * @brief Construct a new PCA9685Driver object.
     *
     * @param i2cAddress The I2C address of PCA9685 (default is 0x40).
     */
    PCA9685Driver(uint8_t i2cAddress = 0x40);

    /**
     * @brief Destroy the PCA9685Driver object.
     */
    ~PCA9685Driver();

    /**
     * @brief Initializes the I2C interfaces and resets PCA9685.
     *
     * @return true if successful.
     * @return false if failed.
     */
    bool begin();

    /**
     * @brief Sets the PWM frequency.
     *
     * @param freqHz The desired PWM frequency in Hz.
     */
    void setPWMFreq(float freqHz);

    /**
     * @brief Sets the pulse width on a specific channel in microseconds.
     *
     * This function converts the pulse width to corresponding counts (0–4095)
     * based on the current frequency, and applies it to the given channel.
     *
     * @param channel The PWM channel (0–15).
     * @param microseconds Pulse width in microseconds.
     */
    void setPulseWidth(uint8_t channel, float microseconds);

private:
    int i2c_fd;      // File descriptor for the I2C device
    uint8_t address; // I2C address of the PCA9685
    float pwmFrequency;

    /**
     * @brief Writes a single byte to a specified PCA9685 register.
     *
     * @param reg The register address.
     * @param value The value to write.
     * @return true if successful.
     * @return false if failed.
     */
    bool write8(uint8_t reg, uint8_t value);

    /**
     * @brief Reads a single byte from a specified PCA9685 register.
     *
     * @param reg The register address.
     * @return The value read from the register.
     */
    uint8_t read8(uint8_t reg);

    /**
     * @brief Sets the raw PWM ON and OFF tick counts for a specific channel.
     *
     * This function directly configures the ON and OFF ticks (0–4095) used
     * by the PCA9685 hardware to generate the PWM signal.
     *
     * @param channel The PWM channel (0–15).
     * @param on The tick count when the signal should turn ON.
     * @param off The tick count when the signal should turn OFF.
     */
    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
};

#endif
