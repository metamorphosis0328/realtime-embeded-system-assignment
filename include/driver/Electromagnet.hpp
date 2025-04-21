#ifndef ELECTROMAGNET_HPP
#define ELECTROMAGNET_HPP

#include "PCA9685Driver.hpp"

#define MAGNET_CHANNEL 14

/**
 * @brief Electromagnet control class using PCA9685.
 *
 * The electromagnet is controlled via PWM signal, similar to servos.
 * 2500us to turn on, 500us to turn off.
 */
class Electromagnet
{
public:
    /**
     * @brief Construct a new Electromagnet object.
     *
     * @param driver Pointer to PCA9685Driver instance.
     * @param channel The PWM channel (0-15).
     */
    Electromagnet(PCA9685Driver *driver, uint8_t channel);

    /**
     * @brief Activate the electromagnet (2500us).
     */
    void activate();

    /**
     * @brief Deactivate the electromagnet (500us).
     */
    void deactivate();

private:
    PCA9685Driver *pca;
    uint8_t channel;
};

#endif
