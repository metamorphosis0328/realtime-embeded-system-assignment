#include "ArmController.hpp"
#include "GomokuAI.hpp"
#include "GomokuVision.hpp"
#include "GomokuCoordinator.hpp"
#include "PCA9685Driver.hpp"
#include "Pump.hpp"
#include "Electromagnet.hpp"
#include <iostream>

// Config
#define LINE_NUM 9
#define BOARD_SIZE 480
#define CAMERA_NUM 0
#define BLACK_PIECE 1
#define WHITE_PIECE 2

ArmController& createArmController()
{
    static PCA9685Driver pca;

    static Servo baseServo(&pca, BASE_SERVO);
    static Servo shoulderServo(&pca, SHOULDER_SERVO);
    static Servo elbowServo(&pca, ELBOW_SERVO);
    static Pump pump(&pca, PUMP_CHANNEL);
    static Electromagnet magnet(&pca, MAGNET_CHANNEL);

    static bool initialized = false;
    if (!initialized) {
        pca.begin();
        pca.setPWMFreq(50);
        initialized = true;
    }

    static ArmController arm(baseServo, shoulderServo, elbowServo, pump, magnet);
    return arm;
}

int main()
{
    try
    {
        // Initialize ai module
        GomokuAI ai(LINE_NUM);

        // Initialize arm module
        ArmController& arm = createArmController();
        arm.initializeServos();

        // Initialize coordinator module
        GomokuCoordinator coordinator(ai, WHITE_PIECE, &arm);

        // Initialize vision module
        GomokuVision vision(CAMERA_NUM, BOARD_SIZE, LINE_NUM);
        vision.registerCallback(&coordinator);

        vision.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
        return -1;
    }
}