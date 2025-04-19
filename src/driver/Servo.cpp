#include "Servo.hpp"
#include <algorithm>

Servo::Servo(PCA9685Driver* driver, const ServoConfig& cfg)
    : pca(driver), config(cfg) {}

void Servo::setAngle(float angle) {
    // Clamp angle within allowed range
    angle = std::clamp(angle, config.minAngle, config.maxAngle);

    // Linear mapping from angle to pulse width
    float ratio = (angle - config.minAngle) / (config.maxAngle - config.minAngle);
    float pulseUs = config.minAnglePulseUs + ratio * (config.maxAnglePulseUs - config.minAnglePulseUs);

    pca->setPulseWidth(config.channel, pulseUs);
}
