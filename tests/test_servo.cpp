#include "PCA9685Driver.hpp"
#include "Servo.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <map>

void autoTest(Servo &servoBase, Servo &servoShoulder, Servo &servoElbow)
{
    servoBase.setAngle(BASE_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoBase.setAngle(BASE_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoBase.setAngle(0); // reset to center
    std::this_thread::sleep_for(std::chrono::seconds(3));

    servoShoulder.setAngle(SHOULDER_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoShoulder.setAngle(SHOULDER_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoShoulder.setAngle(0);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    servoElbow.setAngle(ELBOW_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoElbow.setAngle(ELBOW_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servoElbow.setAngle(0);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void interactiveTest(std::map<int, Servo *> &servoMap, std::map<int, ServoConfig> &configMap)
{
    int choice;
    float angle;

    std::cout << "\n[FREE MODE] Select servo channel:\n"
              << "  0: Elbow\n"
              << "  1: Base\n"
              << "  2: Shoulder\n"
              << "  5: Exit\n"
              << "Enter channel number: ";
    std::cin >> choice;

    if (choice == 5 || !servoMap.count(choice))
    {
        std::cout << "Exiting or invalid channel.\n";
        return;
    }

    const ServoConfig &selectedConfig = configMap[choice];
    Servo *selectedServo = servoMap[choice];

    std::cout << "\nSelected channel: " << choice
              << " (minAngle = " << selectedConfig.minAngle
              << ", maxAngle = " << selectedConfig.maxAngle << ")\n";

    while (true)
    {
        std::cout << "Enter angle (or type 5 to exit this servo): ";
        std::cin >> angle;

        if (angle == 5)
            break;

        selectedServo->setAngle(angle);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main()
{
    PCA9685Driver pwm;
    pwm.begin();
    pwm.setPWMFreq(50);

    Servo servoBase(&pwm, BASE_SERVO);
    Servo servoShoulder(&pwm, SHOULDER_SERVO);
    Servo servoElbow(&pwm, ELBOW_SERVO);

    // Map channel number to servo object
    std::map<int, Servo *> servoMap = {
        {0, &servoElbow},
        {1, &servoBase},
        {2, &servoShoulder}};

    std::map<int, ServoConfig> configMap = {
        {0, ELBOW_SERVO},
        {1, BASE_SERVO},
        {2, SHOULDER_SERVO}};

    int mode;

    std::cout << "Initializing servos to center..." << std::endl;
    servoBase.setAngle(0);
    servoShoulder.setAngle(0);
    servoElbow.setAngle(0);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Select mode:\n"
              << "  [1] Free control mode\n"
              << "  [2] Automatic test mode\n"
              << "Enter: ";
    std::cin >> mode;

    if (mode == 1)
    {
        interactiveTest(servoMap, configMap);
    }
    else if (mode == 2)
    {
        autoTest(servoBase, servoShoulder, servoElbow);
    }
    else
    {
        std::cout << "Invalid mode." << std::endl;
    }

    std::cout << "Releasing all servos..." << std::endl;
    servoBase.setAngle(0);
    servoShoulder.setAngle(0);
    servoElbow.setAngle(0);

    return 0;
}
