#ifndef SERVO_HPP
#define SERVO_HPP

#include "PCA9685Driver.hpp"

/**
 * @brief Configuration for an individual servo.
 */
struct ServoConfig
{
    int channel;         // PCA9685 channel (0–15)
    float minAngle;      // Minimum logical angle (degrees)
    float maxAngle;      // Maximum logical angle (degrees)
    int minAnglePulseUs; // PWM pulse width corresponding to minAngle (us)
    int maxAnglePulseUs; // PWM pulse width corresponding to maxAngle (us)
};

// === Predefined servo configs ===
// Wiring order: channel 0 (elbow), 1 (base), 2 (shoulder)
// The min and max angles are manually set due to mechanical limits
// Pulse width values are calibrated as follows:
// 1800us for center (0°); 1000us for +90°; 2600us for -90°

/**
 * @brief Base servo configuration (rotation around vertical axis).
 * 
 * Positive angle: clockwise, negative angle: counter-clockwise.
 */
const ServoConfig BASE_SERVO = {
    .channel = 1,
    .minAngle = -90.0f,
    .maxAngle = 90.0f,
    .minAnglePulseUs = 2600,
    .maxAnglePulseUs = 1000,
};

/**
 * @brief Shoulder servo configuration (rotation around shoulder joint).
 * 
 * Positive angle: arm moves forward; negative angle: arm moves backward.
 */
const ServoConfig SHOULDER_SERVO = {
    .channel = 2,
    .minAngle = -20.0f,
    .maxAngle = 45.0f,
    .minAnglePulseUs = 1978, // 1800 + (800 * 20° / 90°)
    .maxAnglePulseUs = 1400, // 1800 - (800 * 45° / 90°)
};

/**
 * @brief Elbow servo configuration (rotation around elbow joint).
 * 
 * Positive angle: arm moves up; negative angle: arm moves down.
 */
const ServoConfig ELBOW_SERVO = {
    .channel = 0,
    .minAngle = -50.0f,
    .maxAngle = 0.0f,
    .minAnglePulseUs = 2244, // 1800 + (800 * 50° / 90°)
    .maxAnglePulseUs = 1800,
};

/**
 * @brief Servo control class using PCA9685.
 */
class Servo
{
public:
    /**
     * @brief Construct a new Servo object using predefined config.
     *
     * @param driver Pointer to a PCA9685 driver instance.
     * @param config ServoConfig containing calibration info.
     */
    Servo(PCA9685Driver *driver, const ServoConfig &config);

    /**
     * @brief Set servo to a specified angle.
     * @param angle Angle in degrees (will be clamped to min/max).
     */
    void setAngle(float angle);

private:
    PCA9685Driver *pca;
    ServoConfig config;
};

#endif
