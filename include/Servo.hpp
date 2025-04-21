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
// 1750us for center (0°); 1000us for +90°; 2500us for -90°

/**
 * @brief Base servo configuration (rotation around vertical axis).
 *
 * Positive angle: clockwise, negative angle: counter-clockwise.
 */
const ServoConfig BASE_SERVO = {
    .channel = 1,
    .minAngle = -90.0f,
    .maxAngle = 90.0f,
    .minAnglePulseUs = 2500, // Calibrated max pulse width
    .maxAnglePulseUs = 1000, // Calibrated min pulse width
};

/**
 * @brief Shoulder servo configuration (rotation around shoulder joint).
 *
 * Positive angle: arm moves forward; negative angle: arm moves backward.
 */
const ServoConfig SHOULDER_SERVO = {
    .channel = 2,
    .minAngle = 0.0f,
    .maxAngle = 90.0f,
    .minAnglePulseUs = 1750, // Calibrated center pulse width
    .maxAnglePulseUs = 1000, // Calibrated min pulse width
};

/**
 * @brief Elbow servo configuration (rotation around elbow joint).
 *
 * Positive angle: arm moves down; negative angle: arm moves up.
 * For this servo, angle-to-pulse mapping is reversed compared to others.
 */
const ServoConfig ELBOW_SERVO = {
    .channel = 0,
    .minAngle = 0.0f,
    .maxAngle = 75.0f,
    .minAnglePulseUs = 1750, // Calibrated center pulse width
    .maxAnglePulseUs = 2375, // 1750 + (750 * 75° / 90°)
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

    /**
     * @brief Move servo smoothly to target angle using linear interpolation.
     * @param targetAngle Target angle in degrees. (will be clamped to min/max).
     * @param step Size of each angle increment in degrees (e.g., 10.0f means move in 10° steps).
     * @param delayMs Delay in milliseconds between each step (e.g., 10ms per step).
     */
    void setAngleSmoothly(float targetAngle, float step = 15.0f, int delayMs = 50);

    /**
     * @brief Get the current stored angle of the servo.
     */
    float getAngle() const { return currentAngle; }

private:
    PCA9685Driver *pca;
    ServoConfig config;
    float currentAngle;
};

#endif
