#include "servo/ServoDriver.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <map>

void autoTest(ServoDriver &servo)
{
    servo.setServoAngle(BASE_SERVO, BASE_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.setServoAngle(BASE_SERVO, BASE_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.resetServos(BASE_SERVO);
    std::this_thread::sleep_for(std::chrono::seconds(3));


    servo.setServoAngle(SHOULDER_SERVO, SHOULDER_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.setServoAngle(SHOULDER_SERVO, SHOULDER_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.resetServos(SHOULDER_SERVO);
    std::this_thread::sleep_for(std::chrono::seconds(3));


    servo.setServoAngle(ELBOW_SERVO, ELBOW_SERVO.minAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.setServoAngle(ELBOW_SERVO, ELBOW_SERVO.maxAngle);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    servo.resetServos(ELBOW_SERVO);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void interactiveTest(ServoDriver &servo)
{
    std::map<int, ServoConfig> servoMap = {
        {0, ELBOW_SERVO},
        {1, BASE_SERVO},
        {2, SHOULDER_SERVO}};

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

    const ServoConfig &selectedServo = servoMap[choice];

    std::cout << "\nSelected channel: " << choice
              << " (minAngle = " << selectedServo.minAngle
              << ", maxAngle = " << selectedServo.maxAngle << ")\n";

    while (true)
    {
        std::cout << "Enter angle (or type 5 to exit this servo): ";
        std::cin >> angle;

        if (angle == 5)
            break;

        servo.setServoAngle(selectedServo, angle);
        // Test code for pulsewidth
        // servo.setServoPulseUs(selectedServo.channel, static_cast<int>(angle));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main()
{
    ServoDriver servo;
    int mode;

    servo.resetServos();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Select mode:\n"
              << "  [1] Free control mode\n"
              << "  [2] Automatic test mode\n"
              << "Enter: ";
    std::cin >> mode;

    if (mode == 1)
    {
        interactiveTest(servo);
    }
    else if (mode == 2)
    {
        autoTest(servo);
    }
    else
    {
        std::cout << "Invalid mode." << std::endl;
    }

    std::cout << "Releasing all servos..." << std::endl;
    servo.releaseAllServos();

    return 0;
}
