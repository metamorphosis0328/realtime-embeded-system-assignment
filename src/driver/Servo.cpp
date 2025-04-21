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

void Servo::setAngleSmoothly(float targetAngle, float step, int delayMs)
{
    // Ignore small angle changes below resolution threshold, as servos can't respond to tiny pulse width variations
    const float resolution = 10.0f;
    float current = this->currentAngle;

    // If the angle difference is smaller than resolution, move directly to target
    if (std::abs(targetAngle - current) < resolution)
    {
        setAngle(targetAngle);
        this->currentAngle = targetAngle;
        return;
    }

    step = std::max(step, resolution);

    float direction = (targetAngle > current) ? 1.0f : -1.0f;
    float a = current;

    while (std::abs(a - targetAngle) >= resolution)
    {
        a += step * direction;
        setAngle(a);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    // Final adjustment to reach the exact target angle
    setAngle(targetAngle);
    this->currentAngle = targetAngle;
}
