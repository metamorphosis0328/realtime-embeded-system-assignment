#include "ArmController.hpp"
#include <thread>
#include <chrono>
#include <iostream>

/**
 * @brief Construct a new ArmController object.
 *
 * @param base Servo for the base joint
 * @param shoulder Servo for the shoulder joint
 * @param elbow Servo for the elbow joint
 */
ArmController::ArmController(Servo &base, Servo &shoulder, Servo &elbow)
    : baseServo(base), shoulderServo(shoulder), elbowServo(elbow) {}

/**
 * @brief Initialize the servos to their starting positions (center).
 */
void ArmController::initializeServos()
{
    baseServo.setAngle(0.0f);
    shoulderServo.setAngle(0.0f);
    elbowServo.setAngle(0.0f);
}

/**
 * @brief Set the angle of the base servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setBaseAngle(float angle)
{
    baseServo.setAngle(angle);
}

/**
 * @brief Set the angle of the shoulder servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setShoulderAngle(float angle)
{
    shoulderServo.setAngle(angle);
}

/**
 * @brief Set the angle of the elbow servo.
 *
 * @param angle Desired angle in degrees.
 * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
 */
void ArmController::setElbowAngle(float angle)
{
    elbowServo.setAngle(angle);
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
        { { {-15.0f, 75.0f, 20.0f}, {0.0f, 70.0f, 25.0f}, {15.0f, 75.0f, 20.0f} }, }, // Top
        { { {-15.5f, 65.0f, 30.0f}, {0.0f, 62.5f, 32.5f}, {15.5f, 65.0f, 30.0f} }, }, // Middle
        { { {-25.0f, 30.0f, 65.0f}, {0.0f, 25.0f, 70.0f}, {30.0f, 30.0f, 65.0f} }, }, // Bottom 
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
    const int GRID_SIZE = 9;
    float t_row = static_cast<float>(row) / (GRID_SIZE - 1);
    float t_col = static_cast<float>(col) / (GRID_SIZE - 1);

    auto interpolate = [](const std::tuple<float, float, float>& a,
                          const std::tuple<float, float, float>& b,
                          float t) -> std::tuple<float, float, float> {
        float a1, a2, a3, b1, b2, b3;
        std::tie(a1, a2, a3) = a;
        std::tie(b1, b2, b3) = b;
        return {
            a1 + (b1 - a1) * t,
            a2 + (b2 - a2) * t,
            a3 + (b3 - a3) * t
        };
    };

    std::tuple<float, float, float> top, middle, bottom, final;

    if (t_row <= 0.5f) {
        top = interpolate(cordinatedGrid[0][0], cordinatedGrid[0][1], t_col * 2);
        middle = interpolate(cordinatedGrid[1][0], cordinatedGrid[1][1], t_col * 2);
        final = interpolate(top, middle, t_row * 2);
    } else {
        middle = interpolate(cordinatedGrid[1][0], cordinatedGrid[1][1], t_col * 2);
        bottom = interpolate(cordinatedGrid[2][0], cordinatedGrid[2][1], t_col * 2);
        final = interpolate(middle, bottom, (t_row - 0.5f) * 2);
    }

    return final;
}

void ArmController::loadLookupTable()
{
    angleTable[{0, 0}] = {-15.0f, 75.0f, 20.0f};
    angleTable[{0, 1}] = {-11.2f, 73.8f, 21.2f};
    angleTable[{0, 2}] = {-7.5f, 72.5f, 22.5f};
    angleTable[{0, 3}] = {-3.8f, 71.2f, 23.8f};
    angleTable[{0, 4}] = {0.0f, 70.0f, 25.0f};
    angleTable[{0, 5}] = {3.8f, 71.2f, 23.8f};
    angleTable[{0, 6}] = {7.5f, 72.5f, 22.5f};
    angleTable[{0, 7}] = {11.2f, 73.8f, 21.2f};
    angleTable[{0, 8}] = {15.0f, 75.0f, 20.0f};

    angleTable[{1, 0}] = {-45.0f, 54.4f, 17.6f};
    angleTable[{1, 1}] = {-33.8f, 54.4f, 17.8f};
    angleTable[{1, 2}] = {-22.5f, 54.4f, 17.9f};
    angleTable[{1, 3}] = {-11.2f, 54.4f, 18.0f};
    angleTable[{1, 4}] = {0.0f, 54.4f, 18.1f};
    angleTable[{1, 5}] = {11.2f, 54.4f, 18.2f};
    angleTable[{1, 6}] = {22.5f, 54.4f, 18.4f};
    angleTable[{1, 7}] = {33.8f, 54.4f, 18.5f};
    angleTable[{1, 8}] = {45.0f, 54.4f, 18.6f};

    angleTable[{2, 0}] = {-45.0f, 48.8f, 17.8f};
    angleTable[{2, 1}] = {-33.8f, 48.8f, 17.9f};
    angleTable[{2, 2}] = {-22.5f, 48.8f, 18.0f};
    angleTable[{2, 3}] = {-11.2f, 48.8f, 18.1f};
    angleTable[{2, 4}] = {0.0f, 48.8f, 18.2f};
    angleTable[{2, 5}] = {11.2f, 48.8f, 18.4f};
    angleTable[{2, 6}] = {22.5f, 48.8f, 18.5f};
    angleTable[{2, 7}] = {33.8f, 48.8f, 18.6f};
    angleTable[{2, 8}] = {45.0f, 48.8f, 18.8f};

    angleTable[{3, 0}] = {-45.0f, 43.1f, 17.9f};
    angleTable[{3, 1}] = {-33.8f, 43.1f, 18.0f};
    angleTable[{3, 2}] = {-22.5f, 43.1f, 18.1f};
    angleTable[{3, 3}] = {-11.2f, 43.1f, 18.2f};
    angleTable[{3, 4}] = {0.0f, 43.1f, 18.4f};
    angleTable[{3, 5}] = {11.2f, 43.1f, 18.5f};
    angleTable[{3, 6}] = {22.5f, 43.1f, 18.6f};
    angleTable[{3, 7}] = {33.8f, 43.1f, 18.8f};
    angleTable[{3, 8}] = {45.0f, 43.1f, 18.9f};

    angleTable[{4, 0}] = {-45.0f, 37.5f, 18.0f};
    angleTable[{4, 1}] = {-33.8f, 37.5f, 18.1f};
    angleTable[{4, 2}] = {-22.5f, 37.5f, 18.2f};
    angleTable[{4, 3}] = {-11.2f, 37.5f, 18.4f};
    angleTable[{4, 4}] = {0.0f, 37.5f, 18.5f};
    angleTable[{4, 5}] = {11.2f, 37.5f, 18.6f};
    angleTable[{4, 6}] = {22.5f, 37.5f, 18.8f};
    angleTable[{4, 7}] = {33.8f, 37.5f, 18.9f};
    angleTable[{4, 8}] = {45.0f, 37.5f, 19.0f};

    angleTable[{5, 0}] = {-45.0f, 31.9f, 18.1f};
    angleTable[{5, 1}] = {-33.8f, 31.9f, 18.2f};
    angleTable[{5, 2}] = {-22.5f, 31.9f, 18.4f};
    angleTable[{5, 3}] = {-11.2f, 31.9f, 18.5f};
    angleTable[{5, 4}] = {0.0f, 31.9f, 18.6f};
    angleTable[{5, 5}] = {11.2f, 31.9f, 18.8f};
    angleTable[{5, 6}] = {22.5f, 31.9f, 18.9f};
    angleTable[{5, 7}] = {33.8f, 31.9f, 19.0f};
    angleTable[{5, 8}] = {45.0f, 31.9f, 19.1f};

    angleTable[{6, 0}] = {-45.0f, 26.2f, 18.2f};
    angleTable[{6, 1}] = {-33.8f, 26.2f, 18.4f};
    angleTable[{6, 2}] = {-22.5f, 26.2f, 18.5f};
    angleTable[{6, 3}] = {-11.2f, 26.2f, 18.6f};
    angleTable[{6, 4}] = {0.0f, 26.2f, 18.8f};
    angleTable[{6, 5}] = {11.2f, 26.2f, 18.9f};
    angleTable[{6, 6}] = {22.5f, 26.2f, 19.0f};
    angleTable[{6, 7}] = {33.8f, 26.2f, 19.1f};
    angleTable[{6, 8}] = {45.0f, 26.2f, 19.2f};

    angleTable[{7, 0}] = {-45.0f, 20.6f, 18.4f};
    angleTable[{7, 1}] = {-33.8f, 20.6f, 18.5f};
    angleTable[{7, 2}] = {-22.5f, 20.6f, 18.6f};
    angleTable[{7, 3}] = {-11.2f, 20.6f, 18.8f};
    angleTable[{7, 4}] = {0.0f, 20.6f, 18.9f};
    angleTable[{7, 5}] = {11.2f, 20.6f, 19.0f};
    angleTable[{7, 6}] = {22.5f, 20.6f, 19.1f};
    angleTable[{7, 7}] = {33.8f, 20.6f, 19.2f};
    angleTable[{7, 8}] = {45.0f, 20.6f, 19.4f};

    angleTable[{8, 0}] = {-45.0f, 15.0f, 18.5f};
    angleTable[{8, 1}] = {-33.8f, 15.0f, 18.6f};
    angleTable[{8, 2}] = {-22.5f, 15.0f, 18.8f};
    angleTable[{8, 3}] = {-11.2f, 15.0f, 18.9f};
    angleTable[{8, 4}] = {0.0f, 15.0f, 19.0f};
    angleTable[{8, 5}] = {11.2f, 15.0f, 19.1f};
    angleTable[{8, 6}] = {22.5f, 15.0f, 19.2f};
    angleTable[{8, 7}] = {33.8f, 15.0f, 19.4f};
    angleTable[{8, 8}] = {45.0f, 15.0f, 19.5f};
}

void ArmController::placePieceAt(int row, int col)
{
    auto key = std::make_pair(row, col);

    if (angleTable.find(key) != angleTable.end())
    {
        auto [base, shoulder, elbow] = angleTable[key];

        setBaseAngle(base);
        setShoulderAngle(shoulder);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        setElbowAngle(elbow);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        setElbowAngle(0.0f);
    }
    else
    {
        std::cerr << "[WARN] No calibration for (" << row << ", " << col << ")\n";
    }
}

/**
 * @brief Reset all servos to their initial positions.
 */
void ArmController::resetServos()
{
    baseServo.setAngle(0.0f);
    shoulderServo.setAngle(0.0f);
    elbowServo.setAngle(0.0f);
}
