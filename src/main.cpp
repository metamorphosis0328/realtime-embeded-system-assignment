#include "ArmController.hpp"
#include "GomokuAI.hpp"
#include "GomokuVision.hpp"
#include "GomokuCoordinator.hpp"
#include "PCA9685Driver.hpp"
#include "Pump.hpp"
#include "Electromagnet.hpp"
#include <iostream>
#include <chrono>
#include <thread>

// Config
#define LINE_NUM 9
#define BOARD_SIZE 480
#define CAMERA_NUM 0
#define BLACK_PIECE 1
#define WHITE_PIECE 2

// Create and initialize hardware interfaces
ArmController &createArmController()
{
    static PCA9685Driver pca;

    static Servo baseServo(&pca, BASE_SERVO);
    static Servo shoulderServo(&pca, SHOULDER_SERVO);
    static Servo elbowServo(&pca, ELBOW_SERVO);
    static Pump pump(&pca, PUMP_CHANNEL);
    static Electromagnet magnet(&pca, MAGNET_CHANNEL);

    static bool initialized = false;
    if (!initialized)
    {
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

        while (arm.getStage() != ArmController::Stage::Idle)
        {
            arm.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Minimal delay for pulling
        }
        std::cout << "[MAIN] Arm initialization complete. Starting worker thread...\n";

        // Start arm module thread
        arm.startWorker();

        // Initialize coordinator module
        GomokuCoordinator coordinator(ai, WHITE_PIECE, &arm);

        // Initialize vision module
        GomokuVision vision(CAMERA_NUM, BOARD_SIZE, LINE_NUM);
        vision.registerCallback(&coordinator);

        // Start vision module in a separate thread
        std::thread visionThread(
            [&]()
            {
                try
                {
                    vision.run();
                }
                catch (const std::exception &e)
                {
                    std::cerr << "[Vision Thread Exception] " << e.what() << std::endl;
                    std::abort();
                }
            });

        visionThread.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
        return -1;
    }
}
