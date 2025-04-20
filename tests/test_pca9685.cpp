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

    std::cout << "[PWM Pulse Width Test Mode]\n";
    std::cout << "Enter: <channel> <pulseWidthUs>  (e.g. 0 1500)\n";
    std::cout << "Enter: 55 to exit.\n";

    int channel;
    float pulseWidthUs;

    while (true)
    {
        std::cout << "\n> ";
        std::cin >> channel;

        if (channel == 55)
            break;

        std::cin >> pulseWidthUs;

        if (channel < 0 || channel > 15)
        {
            std::cerr << "Invalid channel (0–15).\n";
            continue;
        }

        if (pulseWidthUs < 400 || pulseWidthUs > 2700)
        {
            std::cerr << "Warning: " << pulseWidthUs << "us may be outside safe range for servos.\n";
        }

        pwm.setPulseWidth(channel, pulseWidthUs);
        std::cout << "Set channel " << channel << " to " << pulseWidthUs << "us.\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "Test session ended.\n";
    return 0;
}