#include "ArmController.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <math.h>

/**
 * @brief Construct a new ArmController object.
 *
 * @param base Servo for the base joint
 * @param shoulder Servo for the shoulder joint
 * @param elbow Servo for the elbow joint
 */
ArmController::ArmController(Servo &base, Servo &shoulder, Servo &elbow, Pump &pump, Electromagnet &magnet)
    : baseServo(base), shoulderServo(shoulder), elbowServo(elbow), pump(pump), magnet(magnet) {}

/**
 * @brief Initialize the servos to their starting positions (center).
 */
void ArmController::initializeServos()
{
    elbowServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    shoulderServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    baseServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * @brief Set the angle of the base servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setBaseAngle(float angle)
{
    baseServo.setAngleSmoothly(angle);
}

/**
 * @brief Set the angle of the shoulder servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setShoulderAngle(float angle)
{
    shoulderServo.setAngleSmoothly(angle);
}

/**
 * @brief Set the angle of the elbow servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setElbowAngle(float angle)
{
    elbowServo.setAngleSmoothly(angle);
}

/**
 * @brief Manually cordinated grid for 9 points
 * Each tuple records a set of servo angles:
 * std::tuple<float, float, float> { baseAngle, shoulderAngle, elbowAngle }
 * The grid is as follows:
 * (0, 0) ... (0, 4) ... (0, 8)
 *  ...
 * (4, 0) ... (4, 4) ... (4, 8)
 *  ...
 * (8, 0) ... (8, 4) ... (8, 8)
 */
const std::array<std::array<std::tuple<float, float, float>, 3>, 3> ArmController::cordinatedGrid =
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
 * Calculate the 3 servo angles via bilinear interpolation to manually cordinated grid.
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

    auto q11 = cordinatedGrid[r0][c0];
    auto q21 = cordinatedGrid[r0][c1];
    auto q12 = cordinatedGrid[r1][c0];
    auto q22 = cordinatedGrid[r1][c1];

    auto top = interpolate(q11, q21, fc);      // left to right on top
    auto bottom = interpolate(q12, q22, fc);   // left to right on bottom
    auto final = interpolate(top, bottom, fr); // top to bottom

    return final;
}

void ArmController::placePieceAt(int row, int col)
{
    if (row < 0 || row > 8 || col < 0 || col > 8)
    {
        std::cerr << "[WARN] Invalid coordinate for interpolation: (" << row << ", " << col << ")\n";
        return;
    }

    auto [base, shoulder, elbow] = interpolateAngles(row, col);

    // Test code
    // std::cout << "[TEST] Placing at board coordinate: (" << row << ", " << col << ")\n";
    // std::cout << "[TEST] Interpolated angles: base=" << base
    //           << ", shoulder=" << shoulder
    //           << ", elbow=" << elbow << "\n";

    setBaseAngle(base);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    setShoulderAngle(shoulder);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    setElbowAngle(elbow);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * @brief Reset all servos to their initial positions.
 */
void ArmController::resetServos()
{
    elbowServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    shoulderServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    baseServo.setAngleSmoothly(0.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * @brief Turn on the pump and activate electromagnet to grip an object.
 */
void ArmController::grip()
{
    pump.turnOn();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    magnet.activate();
}

/**
 * @brief Deactivate the electromagnet and turn off the pump to release an object.
 */
void ArmController::release()
{
    magnet.deactivate();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    pump.turnOff();
}

/**
 * @brief Grip new piece at specific location.
 */
void ArmController::gripNewPiece()
{
    baseServo.setAngleSmoothly(-90.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    shoulderServo.setAngleSmoothly(30.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    elbowServo.setAngleSmoothly(60.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    grip();
}
