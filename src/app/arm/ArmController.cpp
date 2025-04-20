#include "ArmController.hpp"

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
 * @brief Interpolates servo angles based on row/col position using bilinear interpolation.
 * The board is a 9*9 with calibrated corner angles.
 *
 * @param row Row index [0, 8]
 * @param col Column index [0, 8]
 * @return std::pair<float, float> { baseAngle, shoulderAngle }
 */
std::pair<float, float> ArmController::interpolateAngles(int row, int col)
{
    // Normalized position
    const int BOARD_SIZE = 9;
    float t_row = static_cast<float>(row) / (BOARD_SIZE - 1);
    float t_col = static_cast<float>(col) / (BOARD_SIZE - 1);

    // === Calibrated corner values ===
    // Base angles
    float base_tl = -30.0f; // top-left (0, 0)
    float base_tr = 30.0f;  // top-right (0, 8)
    float base_bl = -30.0f; // bottom-left (8, 0)
    float base_br = 30.0f;  // bottom-right (8, 8)

    // Shoulder angles
    float sh_tl = 60.0f; // top-left
    float sh_tr = 60.0f; // top-right
    float sh_bl = 15.0f; // bottom-left
    float sh_br = 15.0f; // bottom-right

    // Interpolate base angle
    float base_top = base_tl * (1 - t_col) + base_tr * t_col;
    float base_bot = base_bl * (1 - t_col) + base_br * t_col;
    float baseAngle = base_top * (1 - t_row) + base_bot * t_row;

    // Interpolate shoulder angle
    float sh_top = sh_tl * (1 - t_col) + sh_tr * t_col;
    float sh_bot = sh_bl * (1 - t_col) + sh_br * t_col;
    float shoulderAngle = sh_top * (1 - t_row) + sh_bot * t_row;

    return {baseAngle, shoulderAngle};
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
