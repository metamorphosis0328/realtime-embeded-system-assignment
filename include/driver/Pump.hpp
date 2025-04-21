#ifndef PUMP_HPP
#define PUMP_HPP

#include "PCA9685Driver.hpp"

/**
 * @brief Pump control class using PCA9685.
 *
 * The pump is controlled via PWM signal, similar to servos.
 * 2500us to turn on, 500us to turn off.
 */
class Pump
{
public:
    /**
     * @brief Construct a new Pump object.
     *
     * @param driver Pointer to PCA9685Driver instance.
     * @param channel The PWM channel (0-15).
     */
    Pump(PCA9685Driver *driver, uint8_t channel);

    /**
     * @brief Turn the pump on (2500us).
     */
    void turnOn();

    /**
     * @brief Turn the pump off (500us).
     */
    void turnOff();

private:
    PCA9685Driver *pca;
    uint8_t channel;
};

#endif
