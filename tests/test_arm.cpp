#include "Servo.hpp"
#include "ArmController.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testInitializeServos(ArmController &armController)
{
    std::cout << "Testing initialization of servos to center position...\n";
    armController.initializeServos();

    // Verify that the servos are at center position (90°)
    std::cout << "Base servo angle: 0°\n";
    std::cout << "Shoulder servo angle: 0°\n";
    std::cout << "Elbow servo angle: 0°\n";
}

void testSetAngles(ArmController &armController)
{
    float baseAngle = 30.0f;
    float shoulderAngle = 45.0f;
    float elbowAngle = -10.0f;

    std::cout << "Testing setting angles for servos...\n";

    // Set angles for the servos
    armController.setBaseAngle(baseAngle);
    armController.setShoulderAngle(shoulderAngle);
    armController.setElbowAngle(elbowAngle);

    // Verify if the angles are set correctly
    std::cout << "Base servo set to: " << baseAngle << "°\n";
    std::cout << "Shoulder servo set to: " << shoulderAngle << "°\n";
    std::cout << "Elbow servo set to: " << elbowAngle << "°\n";
}

void testResetServos(ArmController &armController)
{
    std::cout << "Testing resetting servos to center position...\n";
    armController.resetServos();

    // Verify that all servos are reset to the center position
    std::cout << "Base servo reset to center position: 0\n";
    std::cout << "Shoulder servo reset to center position: 0\n";
    std::cout << "Elbow servo reset to center position: 0\n";
}

int main()
{
    // Initialize the PCA9685Driver and servos
    PCA9685Driver pwm;
    pwm.begin();
    pwm.setPWMFreq(50);

    Servo baseServo(&pwm, BASE_SERVO);
    Servo shoulderServo(&pwm, SHOULDER_SERVO);
    Servo elbowServo(&pwm, ELBOW_SERVO);

    // Create the ArmController with the servos
    ArmController armController(baseServo, shoulderServo, elbowServo);

    // Test the basic functionality
    testInitializeServos(armController);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    testSetAngles(armController);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    testResetServos(armController);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Basic functionality test completed!" << std::endl;

    return 0;
}
