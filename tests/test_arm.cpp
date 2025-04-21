#include "Servo.hpp"
#include "ArmController.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void autoTest(ArmController &armController)
{
    std::cout << "Testing initialization of servos to center position...\n";
    armController.initializeServos();

    // Verify that the servos are at center position (90°)
    std::cout << "Base servo angle: 0°\n";
    std::cout << "Shoulder servo angle: 0°\n";
    std::cout << "Elbow servo angle: 0°\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Test setting angles for the servos
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

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Test resetting servos to center position
    std::cout << "Testing resetting servos to center position...\n";
    armController.resetServos();

    // Verify that all servos are reset to the center position
    std::cout << "Base servo reset to center position: 0\n";
    std::cout << "Shoulder servo reset to center position: 0\n";
    std::cout << "Elbow servo reset to center position: 0\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void singleTest(ArmController &armController)
{
    int choice;
    float angle;

    while (true) // Main menu for servo selection
    {
        std::cout << "\n[Single Servo Mode] Select servo:\n"
                  << "  0: Base\n"
                  << "  1: Shoulder\n"
                  << "  2: Elbow\n"
                  << "  5: Exit\n"
                  << "Enter servo number: ";
        std::cin >> choice;

        if (choice == 5)
        {
            std::cout << "Exiting manual mode.\n";
            break;
        }

        // Map user input to servo control
        switch (choice)
        {
        case 0:
            std::cout << "Control Base Servo\n";
            break;
        case 1:
            std::cout << "Control Shoulder Servo\n";
            break;
        case 2:
            std::cout << "Control Elbow Servo\n";
            break;
        default:
            std::cout << "Invalid selection.\n";
            continue; // Back to main menu
        }

        while (true) // Submenu for angle setting
        {
            std::cout << "Enter angle (or type 5 to return to main menu): ";
            std::cin >> angle;

            if (angle == 5)
            {
                break; // Back to main menu
            }

            switch (choice)
            {
            case 0:
                armController.setBaseAngle(angle);
                std::cout << "Base servo set to: " << angle << "°\n";
                break;
            case 1:
                armController.setShoulderAngle(angle);
                std::cout << "Shoulder servo set to: " << angle << "°\n";
                break;
            case 2:
                armController.setElbowAngle(angle);
                std::cout << "Elbow servo set to: " << angle << "°\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void freeTest(ArmController &armController)
{
    int servoID;
    float angle;

    std::cout << "\n[Free Mode] Control servos by typing: <servo_id> <angle>\n";
    std::cout << "Servo IDs: 0 = Base, 1 = Shoulder, 2 = Elbow\n";
    std::cout << "Type 5 to exit this mode.\n";

    while (true)
    {
        std::cout << "\nEnter servo ID and angle: ";
        std::cin >> servoID;

        if (servoID == 5)
        {
            std::cout << "Exiting Direct Input Mode.\n";
            break;
        }

        std::cin >> angle;

        switch (servoID)
        {
        case 0:
            armController.setBaseAngle(angle);
            std::cout << "Base set to: " << angle << "°\n";
            break;
        case 1:
            armController.setShoulderAngle(angle);
            std::cout << "Shoulder set to: " << angle << "°\n";
            break;
        case 2:
            armController.setElbowAngle(angle);
            std::cout << "Elbow set to: " << angle << "°\n";
            break;
        default:
            std::cout << "Invalid servo ID. Please use 0 (Base), 1 (Shoulder), or 2 (Elbow).\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void interpolationTest(ArmController &armController)
{
    int row, col;

    std::cout << "[Interpolation Test] Input a board coordinate to compute angles.\n";
    std::cout << "Enter \"55 5\" to quit.\n";

    while (true)
    {
        std::cout << "Enter coordinate (row col): ";
        std::cin >> row >> col;

        if (row == 55 && col == 5)
            break;

        if (row < 0 || row > 8 || col < 0 || col > 8)
        {
            std::cout << "Invalid input. Row and column must be between 0 and 8.\n";
            continue;
        }

        auto [base, shoulder, elbow] = armController.interpolateAngles(row, col);

        std::cout << "Interpolated base angle: " << base << "°\n";
        std::cout << "Interpolated shoulder angle: " << shoulder << "°\n";
        std::cout << "Interpolated elbow angle: " << elbow << "°\n";

        armController.setBaseAngle(base);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        armController.setShoulderAngle(shoulder);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        armController.setElbowAngle(elbow);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        armController.resetServos();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Exiting interpolation test.\n";
}

void lookupTest(ArmController &armController)
{
    int row, col;
    std::cout << "[Lookup Table Test] Input a board coordinate (row col) to test lookup-based movement.\n";
    std::cout << "Enter \"55 5\" to quit.\n";

    while (true)
    {
        std::cout << "Enter coordinate (row col): ";
        std::cin >> row >> col;

        if (row == 55 && col == 5)
        {
            std::cout << "Exiting lookup test.\n";
            break;
        }

        if (row < 0 || row > 8 || col < 0 || col > 8)
        {
            std::cout << "Invalid input. Row and column must be between 0 and 7.\n";
            continue;
        }

        std::cout << "Executing movement using angleTable...\n";
        armController.placePieceAt(row, col);
    }
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

    int mode;

    std::cout << "Select mode:\n"
              << "  [0] Free control mode\n"
              << "  [1] Single servo control mode\n"
              << "  [2] Automatic test mode\n"
              << "  [3] Interpolation test mode\n"
              << "  [4] Lookup table test mode\n"
              << "Enter: ";
    std::cin >> mode;

    if (mode == 0)
    {
        freeTest(armController);
    }
    else if (mode == 1)
    {
        singleTest(armController);
    }
    else if (mode == 2)
    {
        autoTest(armController);
    }
    else if (mode == 3)
    {
        interpolationTest(armController);
    }
    else if (mode == 4)
    {
        armController.loadLookupTable();
        lookupTest(armController);
    }
    else
    {
        std::cout << "Invalid mode selection." << std::endl;
    }

    std::cout << "Test completed!" << std::endl;

    return 0;
}