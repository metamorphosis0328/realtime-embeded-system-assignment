#ifndef ARMCONTROLLER_HPP
#define ARMCONTROLLER_HPP

#include "Servo.hpp"

class ArmController
{
public:
    /**
     * @brief Construct a new ArmController object.
     *
     * @param base Servo for the base joint
     * @param shoulder Servo for the shoulder joint
     * @param elbow Servo for the elbow joint
     */
    ArmController(Servo &base, Servo &shoulder, Servo &elbow);

    /**
     * @brief Initialize the servos to their starting positions (center).
     */
    void initializeServos();

    /**
     * @brief Set the angle of the base servo (rotation around vertical axis).
     *
     * @param angle Desired angle in degrees. Positive: arm moves clockwise; negative: arm moves counter-clockwise.
     * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
     */
    void setBaseAngle(float angle);

    /**
     * @brief Set the angle of the shoulder servo (rotation around shoulder joint).
     *
     * @param angle Desired angle in degrees. Positive: arm moves forward; negative: arm moves backward.
     * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
     */
    void setShoulderAngle(float angle);

    /**
     * @brief Set the angle of the elbow servo (rotation around elbow joint).
     *
     * @param angle Desired angle in degrees. Positive: arm moves up; negative: arm moves down.
     * (The angle will be clamped within the servo's physical limits. See Servo.hpp for details)
     */
    void setElbowAngle(float angle);

    /**
     * @brief Reset all servos to their initial positions.
     */
    void resetServos();

private:
    Servo &baseServo;
    Servo &shoulderServo;
    Servo &elbowServo;
};

#endif
