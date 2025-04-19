#include "PCA9685Driver.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    PCA9685Driver pwm;

    if (!pwm.begin())
    {
        std::cerr << "Failed to initialize PCA9685." << std::endl;
        return 1;
    }

    pwm.setPWMFreq(50);

    int channel;
    std::cout << "Enter the PCA9685 channel to test (0–15): ";
    std::cin >> channel;

    if (channel < 0 || channel > 15)
    {
        std::cerr << "Invalid channel number. Must be between 0 and 15." << std::endl;
        return 1;
    }

    const float PWM_ON_US = 2500.0f;
    const float PWM_OFF_US = 500.0f;

    std::cout << "Testing channel " << channel << " with pulse widths 2500us and 500us" << std::endl;

    try
    {
        while (true)
        {
            std::cout << "Setting pulse width to 2500us (ON)" << std::endl;
            pwm.setPulseWidth(channel, PWM_ON_US);
            std::this_thread::sleep_for(std::chrono::seconds(3));

            std::cout << "Setting pulse width to 500us (OFF)" << std::endl;
            pwm.setPulseWidth(channel, PWM_OFF_US);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    catch (...)
    {
        std::cerr << "Exception caught. Exiting test." << std::endl;
        return 1;
    }

    return 0;
}
