#include "Servo.hpp"
#include <algorithm>
#include <thread>

Servo::Servo(PCA9685Driver *driver, const ServoConfig &config)
    : pca(driver), config(config), currentAngle(0.0f) {}

void Servo::setAngle(float angle)
{
    // Clamp angle within allowed range
    angle = std::clamp(angle, config.minAngle, config.maxAngle);

    // Linear mapping from angle to pulse width
    float ratio = (angle - config.minAngle) / (config.maxAngle - config.minAngle);
    float pulseUs = config.minAnglePulseUs + ratio * (config.maxAnglePulseUs - config.minAnglePulseUs);

    pca->setPulseWidth(config.channel, pulseUs);
}
