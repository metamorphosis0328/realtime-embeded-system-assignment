#include "Pump.hpp"

/**
 * @brief Construct a new Pump object.
 *
 * @param driver Pointer to PCA9685Driver instance.
 * @param channel PWM channel (0-15).
 */
Pump::Pump(PCA9685Driver *driver, uint8_t channel)
    : pca(driver), channel(channel) {}

/**
 * @brief Turn the pump on (2500us).
 */
void Pump::turnOn()
{
    pca->setPulseWidth(channel, 2500); // Turn on (2500us)
}

/**
 * @brief Turn the pump off (500us).
 */
void Pump::turnOff()
{
    pca->setPulseWidth(channel, 500); // Turn off (500us)
}
