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
 * @brief Reset all servos to their initial positions.
 */
void ArmController::resetServos()
{
    baseServo.setAngle(0.0f);
    shoulderServo.setAngle(0.0f);
    elbowServo.setAngle(0.0f);
}
