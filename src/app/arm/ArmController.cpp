#include "ArmController.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

/**
 * @brief Construct a new ArmController object.
 *
 * @param base Servo for the base joint
 * @param shoulder Servo for the shoulder joint
 * @param elbow Servo for the elbow joint
 */
ArmController::ArmController(Servo &base, Servo &shoulder, Servo &elbow, Pump &pump, Electromagnet &magnet)
    : baseServo(base), shoulderServo(shoulder), elbowServo(elbow), pump(pump), magnet(magnet), currentStage(Stage::Idle) {}

/**
 * @brief Initialize the servos to their starting positions (center).
 */
void ArmController::initializeServos()
{
    stageStartTime = std::chrono::steady_clock::now();
    currentStage = Stage::ResetStartElbow;
    isInit = true;
}

/**
 * @brief Manually coordinated grid for 9 points
 * Each tuple records a set of servo angles:
 * std::tuple<float, float, float> { baseAngle, shoulderAngle, elbowAngle }
 * The grid is as follows:
 * (0, 0) ... (0, 4) ... (0, 8)
 *  ...
 * (4, 0) ... (4, 4) ... (4, 8)
 *  ...
 * (8, 0) ... (8, 4) ... (8, 8)
 */
const std::array<std::array<std::tuple<float, float, float>, 3>, 3> ArmController::coordinatedGrid =
    {{
    {
        {{-15.0f, 75.0f, 20.0f}, {0.0f, 70.0f, 25.0f}, {15.0f, 75.0f, 20.0f}}, // Top
    },
    {
        {{-22.5f, 45.0f, 55.0f}, {0.0f, 45.0f, 55.0f}, {20.0f, 45.0f, 55.0f}}, // Middle
    },
    {
        {{-25.0f, 30.0f, 65.0f}, {0.0f, 25.0f, 70.0f}, {30.0f, 30.0f, 65.0f}}, // Bottom
    },
}};

/**
 * @brief Interpolates servo angles based on row/col position using bilinear interpolation.
 * Calculate the 3 servo angles via bilinear interpolation to manually coordinated grid.
 *
 * @param row Row index [0, 8]
 * @param col Column index [0, 8]
 * @return std::tuple<float, float, float> { baseAngle, shoulderAngle, elbowAngle }
 */
std::tuple<float, float, float> ArmController::interpolateAngles(int row, int col)
{
    constexpr int GRID_SIZE = 9;
    constexpr int GRID_DIV = 3;

    float t_row = static_cast<float>(row) / (GRID_SIZE - 1); // 0.0 ~ 1.0
    float t_col = static_cast<float>(col) / (GRID_SIZE - 1); // 0.0 ~ 1.0

    float gridRowF = t_row * (GRID_DIV - 1); // scale to 0.0 ~ 2.0
    float gridColF = t_col * (GRID_DIV - 1); // same here

    int r0 = static_cast<int>(std::floor(gridRowF));
    int c0 = static_cast<int>(std::floor(gridColF));
    int r1 = std::min(r0 + 1, GRID_DIV - 1);
    int c1 = std::min(c0 + 1, GRID_DIV - 1);

    float fr = gridRowF - r0; // fractional row part
    float fc = gridColF - c0; // fractional col part

    auto interpolate = [](const std::tuple<float, float, float> &a,
                          const std::tuple<float, float, float> &b,
                          float t) -> std::tuple<float, float, float>
    {
        float a1, a2, a3, b1, b2, b3;
        std::tie(a1, a2, a3) = a;
        std::tie(b1, b2, b3) = b;
        return {
            a1 + (b1 - a1) * t,
            a2 + (b2 - a2) * t,
            a3 + (b3 - a3) * t};
    };

    auto q11 = coordinatedGrid[r0][c0];
    auto q21 = coordinatedGrid[r0][c1];
    auto q12 = coordinatedGrid[r1][c0];
    auto q22 = coordinatedGrid[r1][c1];

    auto top = interpolate(q11, q21, fc);      // Left to right on top
    auto bottom = interpolate(q12, q22, fc);   // Left to right on bottom
    auto final = interpolate(top, bottom, fr); // Top to bottom

    return final;
}

/**
 * @brief Place piece at targetting location.
 *
 * @param row Board row (arm coordinate system)
 * @param col Board col (arm coordinate system)
 */
void ArmController::placePieceAt(int row, int col)
{
    if (row < 0 || row > 8 || col < 0 || col > 8)
    {
        std::cerr << "[WARN] Invalid coordinate for interpolation: (" << row << ", " << col << ")\n";
        return;
    }

    std::tie(targetBase, targetShoulder, targetElbow) = interpolateAngles(row, col);
    stageStartTime = std::chrono::steady_clock::now();
    currentStage = Stage::PlaceStartBase;
}

/**
 * @brief Periodically update the arm placement sequence in a non-blocking way.
 *
 */
void ArmController::update()
{
    using namespace std::chrono;
    auto now = steady_clock::now();

    switch (currentStage)
    {
    // Idle
    case Stage::Idle:
        break;
    // Grip
    case Stage::GripStartBase:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            baseServo.setAngle(-90.0f);
            stageStartTime = now;
            currentStage = Stage::GripMoveShoulder;
        }
        break;
    case Stage::GripMoveShoulder:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            shoulderServo.setAngle(30.0f);
            stageStartTime = now;
            currentStage = Stage::GripMoveElbow;
        }
        break;
    case Stage::GripMoveElbow:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            elbowServo.setAngle(60.0f);
            grip(); // Do grip()
            stageStartTime = now;
            currentStage = Stage::ResetStartElbow;
        }
        break;

    // Place
    case Stage::PlaceStartBase:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            baseServo.setAngle(targetBase);
            stageStartTime = now;
            currentStage = Stage::PlaceMoveShoulder;
        }
        break;
    case Stage::PlaceMoveShoulder:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            shoulderServo.setAngle(targetShoulder);
            stageStartTime = now;
            currentStage = Stage::PlaceMoveElbow;
        }
        break;
    case Stage::PlaceMoveElbow:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            elbowServo.setAngle(targetElbow);
            release(); // Do release()
            stageStartTime = now;
            currentStage = Stage::ResetStartElbow;
        }
        break;

    // Reset (reversed order)
    case Stage::ResetStartElbow:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 800)
        {
            elbowServo.setAngle(0.0f);
            stageStartTime = now;
            currentStage = Stage::ResetMoveShoulder;
        }
        break;
    case Stage::ResetMoveShoulder:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            shoulderServo.setAngle(0.0f);
            stageStartTime = now;
            currentStage = Stage::ResetMoveBase;
        }
        break;
    case Stage::ResetMoveBase:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 500)
        {
            baseServo.setAngle(0.0f);
            stageStartTime = now;
            currentStage = Stage::CompleteWait;
        }
        break;

    // Complete
    case Stage::CompleteWait:
        if (duration_cast<milliseconds>(now - stageStartTime).count() >= 100)
        {
            if (isInit)
            {
                std::cout << "[ARM] Initialization complete.\n";
                currentStage = Stage::Idle;
                isInit = false;
            }
            else
            {
                currentStage = Stage::Completed;
            }
        }
        break;
    case Stage::Completed:
        break;
    }
}

/**
 * @brief Reset all servos to their initial positions.
 */
void ArmController::resetServos()
{
    stageStartTime = std::chrono::steady_clock::now();
    currentStage = Stage::ResetStartElbow;
}

/**
 * @brief Turn on the pump and activate electromagnet to grip an object.
 */
void ArmController::grip()
{
    pump.turnOn();
    magnet.activate();
}

/**
 * @brief Deactivate the electromagnet and turn off the pump to release an
 * object.
 */
void ArmController::release()
{
    magnet.deactivate();
    pump.turnOff();
}

/**
 * @brief Grip new piece at specific location.
 */
void ArmController::gripNewPiece()
{
    stageStartTime = std::chrono::steady_clock::now();
    currentStage = Stage::GripStartBase;
}

/**
 * @brief Starts the background thread for executing arm tasks.
 */
void ArmController::startWorker()
{
    workerThread = std::thread(&ArmController::workerLoop, this);
}

/**
 * @brief Adds a move task to the execution queue.
 *
 * @param row Board row (arm coordinate system)
 * @param col Board col (arm coordinate system)
 */
void ArmController::enqueueMove(int row, int col)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.emplace(row, col);
    cv.notify_one();
}

/**
 * @brief Background thread loop for handling queued arm tasks.
 */
void ArmController::workerLoop()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [&] { return !taskQueue.empty(); });
        if (!running)
            return;

        auto [row, col] = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        // gripNewPiece();
        // while (running && currentStage != Stage::Completed)
        // {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Minimal delay for polling
        //     update();                                                   // Polling
        // }

        placePieceAt(row, col);
        while (running && currentStage != Stage::Completed)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Minimal delay for polling
            update();                                                   // Polling
        }

        resetServos();
    }
}
