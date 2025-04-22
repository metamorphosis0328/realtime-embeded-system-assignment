#ifndef ARMCONTROLLER_HPP
#define ARMCONTROLLER_HPP

#include "Servo.hpp"
#include "Pump.hpp"
#include "Electromagnet.hpp"
#include <array>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

class ArmController
{
public:
    /**
     * @brief Construct a new ArmController object.
     *
     * @param base Servo for the base joint (channel 1)
     * @param shoulder Servo for the shoulder joint (channel 2)
     * @param elbow Servo for the elbow joint (channel 0)
     * @param pump Pump (channel 15)
     * @param magnet Electromagnet (channel 14)
     */
    ArmController(Servo &base, Servo &shoulder, Servo &elbow, Pump &pump, Electromagnet &magnet);

    // Non-blocking action stage
    enum class Stage
    {
        // Idle
        Idle,
        // Place
        PlaceStartBase,
        PlaceMoveShoulder,
        PlaceMoveElbow,
        // Grip
        GripStartBase,
        GripMoveShoulder,
        GripMoveElbow,
        // Reset (reversed order)
        ResetStartElbow,
        ResetMoveShoulder,
        ResetMoveBase,
        // Compelete
        CompleteWait,
        Completed
    };

    /**
     * @brief Get the currentStage.
     * 
     * @return currentStage 
     */
    Stage getStage() const { return currentStage; }

    /**
     * @brief Initialize the servos to their starting positions (center).
     */
    void initializeServos();

    /**
     * @brief Reset all servos to their initial positions.
     */
    void resetServos();

    /**
     * @brief Interpolates servo angles based on row/col position using bilinear interpolation.
     * Calculate the 3 servo angles via bilinear interpolation to manually cordinated grid.
     *
     * @param row Row index on the board (0–8). 0 is the top row, 8 is the bottom.
     * @param col Col index on the board (0–8). 0 is the leftmost column, 8 is the rightmost.
     * @return std::tuple<float, float, float> { baseAngle, shoulderAngle, elbowAngle }
     */
    std::tuple<float, float, float> interpolateAngles(int row, int col);

    void placePieceAt(int row, int col);

    /**
     * @brief Turn on the pump and activate electromagnet to grip an object.
     */
    void grip();

    /**
     * @brief Deactivate the electromagnet and turn off the pump to release an object.
     */
    void release();

    /**
     * @brief Grip new piece at specific location.
     */
    void gripNewPiece();

    /**
     * @brief Starts the background worker thread for executing arm movement tasks.
     */
    void startWorker();

    /**
     * @brief Enqueues a new move task to be executed by the arm.
     *
     * @param row Board row index
     * @param col Board column index
     */
    void enqueueMove(int row, int col);

    /**
     * @brief Called regularly to advance non-blocking arm action
     */
    void update();

private:
    // Servo control components
    Servo &baseServo;
    Servo &shoulderServo;
    Servo &elbowServo;
    static const std::array<std::array<std::tuple<float, float, float>, 3>, 3> coordinatedGrid;

    // Pump and electromagnetic components
    Pump &pump;
    Electromagnet &magnet;

    // Worker thread for asynchronously executing arm movement tasks
    std::thread workerThread;
    std::queue<std::pair<int, int>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool running = true;

    void workerLoop();

    bool isInit = false;
    Stage currentStage = Stage::Idle;
    std::chrono::steady_clock::time_point stageStartTime;
    float targetBase = 0.0f;
    float targetShoulder = 0.0f;
    float targetElbow = 0.0f;
};

#endif
