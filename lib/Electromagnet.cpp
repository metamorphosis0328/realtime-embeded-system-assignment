#include "Electromagnet.hpp"

/**
 * @brief Construct a new Electromagnet object.
 *
 * @param driver Pointer to PCA9685Driver instance.
 * @param channel PWM channel (0-15).
 */
Electromagnet::Electromagnet(PCA9685Driver *driver, uint8_t channel)
    : pca(driver), channel(channel) {}

/**
 * @brief Activate the electromagnet (2500us).
 */
void Electromagnet::activate()
{
    pca->setPulseWidth(channel, 2500); // Turn on (2500us)
}

/**
 * @brief Deactivate the electromagnet (500us).
 */
void Electromagnet::deactivate()
{
    pca->setPulseWidth(channel, 500); // Turn off (500us)
}
