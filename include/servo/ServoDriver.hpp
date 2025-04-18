#ifndef SERVODRIVER_H
#define SERVODRIVER_H

#include <string>

/**
 * @brief Configuration for individual servo channels.
 * Maps a logical angle range (in degrees) to PWM pulse width (in microseconds).
 */
struct ServoConfig
{
    int channel;    // PCA9685 channel number (0–15)
    float minAngle; // Minimum logical angle (degrees)
    float maxAngle; // Maximum logical angle (degrees)
    int minPulseUs; // PWM pulse width corresponding to minAngle (µs)
    int maxPulseUs; // PWM pulse width corresponding to maxAngle (µs)
    bool inverted = true;
};

// === Predefined servo configs ===
// Wiring order: channel 0 (elbow), 1 (base), 2 (shoulder)
// 1800 is calibrated center for each servo

/**
 * @brief Base servo configuration (rotation around vertical axis).
 * Calibrated center position: 1800us.
 * Positive angle: clockwise, negative angle: counter-clockwise
 */
const ServoConfig BASE_SERVO = {
    .channel = 1,
    .minAngle = -90.0f,
    .maxAngle = 90.0f,
    .minPulseUs = 1000,
    .maxPulseUs = 2600,
    .inverted = true};

/**
 * @brief Shoulder servo configuration (rotation around horizontal axis).
 * Calibrated center position: 1800us.
 * Positive angle: arm moves forward; negative angle: arm moves backward.
 */
const ServoConfig SHOULDER_SERVO = {
    .channel = 2,
    .minAngle = -45.0f,
    .maxAngle = 30.0f,
    .minPulseUs = 1400, // 1800 - (800 * 45 / 90)
    .maxPulseUs = 2066, // 1800 + (800 * 30 / 90)
    .inverted = true};

/**
 * @brief Elbow servo configuration (rotation around horizontal axis).
 * Calibrated center position: 1800us.
 * Positive angle: arm moves up; negative angle: arm moves down.
 */
const ServoConfig ELBOW_SERVO = {
    .channel = 0,
    .minAngle = -30.0f,
    .maxAngle = 50.0f,
    .minPulseUs = 1533, // 1800 - (800 * 30 / 90)
    .maxPulseUs = 2240, // 1800 + (800 * 50 / 90)
    .inverted = true};

/**
 * @brief Class to control servo motors via PCA9685 over I2C.
 * Supports setting PWM pulse width or logical angle based on ServoConfig.
 */
class ServoDriver
{
public:
    /**
     * @brief Construct a ServoDriver object.
     * @param i2cBus I2C bus number (default: 1)
     * @param address I2C address of PCA9685 (default: 0x40)
     */
    ServoDriver(int i2cBus = 1, int address = 0x40);

    /**
     * @brief Destructor: closes I2C connection.
     */
    ~ServoDriver();

    /**
     * @brief Set servo pulse width manually (in microseconds).
     * @param channel PCA9685 channel (0–15)
     * @param pulseUs Pulse width in microseconds (usually 500–2500us)
     */
    void setServoPulseUs(int channel, int pulseUs);

    /**
     * @brief Set servo position by logical angle.
     * Uses mapping from ServoConfig.
     * @param config ServoConfig struct
     * @param angleDeg Desired angle in degrees
     */
    void setServoAngle(const ServoConfig &config, float angleDeg);

    /**
     * @brief Release all servos by setting their PWM to 0.
     */
    void releaseAllServos();

private:
    int i2cFd;   // File descriptor for I2C device
    int i2cAddr; // I2C address of the PCA9685 device

    void writeRegister(uint8_t reg, uint8_t value);    // Low-level I2C register write
    void setPWMFreq(int freqHz);                       // Set PCA9685 PWM frequency
    void setPWM(int channel, int onTick, int offTick); // Set raw PWM values
};

#endif
