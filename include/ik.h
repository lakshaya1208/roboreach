#ifndef ROBOREACH_IK_H
#define ROBOREACH_IK_H

#include <Arduino.h>

/**
 * 2-DOF Robotic Arm Inverse Kinematics
 * 
 * Arm layout:
 * 1. Shoulder: Primary lift joint (Annimos 60kg, pitch)
 * 2. Elbow: Forearm elevation joint (20kg servo, pitch)
 * 3. Gripper: End effector clamp (SG90)
 * 
 * Arm geometry:
 *   L1 = 150 mm (shoulder to elbow)
 *   L2 = 120 mm (elbow to gripper center)
 * 
 * All lengths in millimeters, angles in degrees.
 */

// *****************************************************************************
// ***                          Configuration Constants                        ***
// *****************************************************************************

/**
 * Arm link lengths - MUST be calibrated to physical arm measurement.
 * Values per task specification; verify with actual arm.
 */
#define IK_LINK_SHOULDER_TO_ELBOW   (150.0f)  // Shouldard to elbow joint (mm)
#define IK_LINK_ELBOW_TO_GRIPPER    (120.0f)  // Elbow to gripper center (mm)

// *****************************************************************************
// ***                            Data Structures                             ***
// *****************************************************************************

/**
 * 2D target position for the elbow joint (the point where elbow pivot is).
 * Operates in the XZ vertical plane (shoulder at origin after waist rotation).
 */
struct IktargetPosition {
    float x;   // Forward (+) distance from shoulder (mm)
    float z;   // Up (+) distance from shoulder (mm)
};

/**
 * Calculated joint angles for the 2-DOF arm.
 * All angles in degrees, range: 0-180 per servo.
 */
struct IKJointAngles {
    float shoulder;  // Shoulder pitch, 0-180° (positive = arm lifts up)
    float elbow;     // Elbow bend, 0-180° (positive = arm extends forward)
    float gripper;   // Gripper angle, 0-90° (0=open, 90=closed)
};

/**
 * Full arm pose combining elbow position and joint angles.
 */
struct IKPose {
    IktargetPosition elbowPos;  // Elbow joint position in 2D space
    IKJointAngles angles;       // Calculated joint angles
};

/**
 * IK status return code.
 */
typedef enum {
    IK_SUCCESS      = 0,   // Inverse kinematics solved successfully
    IK_OUT_OF_REACH = 1,   // Target position is outside arm workspace
    IK_INVALID_ARG  = 2    // Invalid input parameters
} IKStatus;

/**
 * @brief Initialize IK solver with arm geometry configuration.
 * @param shoulderToElbow Shoulder to elbow link length (mm)
 * @param elbowToGripper Elbow to gripper link length (mm)
 * @return IKStatus IK_SUCCESS if config is valid, IK_INVALID_ARG otherwise.
 */
IKStatus ikInit(float shoulderToElbow, float elbowToGripper);

/**
 * @brief Calculate inverse kinematics for 2-DOF arm.
 * 
 * Given a target elbow position (x, z) in the vertical plane relative to
 * the shoulder base, calculate the shoulder and elbow angles needed to
 * reach that position.
 * 
 * @param[in]  pos   Target elbow position in 2D space (mm)
 * @param[out] angles  Calculated joint angles (output)
 * @param[out] pose    Full pose including elbow position and angles (output)
 * @return IKStatus Status of the calculation.
 */
IKStatus ikCalculate(const IktargetPosition* pos, IKJointAngles* angles, IKPose* pose);

/**
 * @brief Map existing angle-based controls (0-180°) to IK target position.
 * 
 * Converts the RoboLink control values (arm_x for shoulder, arm_y for elbow)
 * into a wrist/elbow target position that the 2-DOF IK can use.
 * 
 * @param shoulderDeg Shoulder angle from RoboLink (0-180)
 * @param elbowDeg    Elbow angle from RoboLink (0-180)
 * @param[out] pos    Computed elbow target position (output)
 * @return true if calculation valid, false if out of workspace.
 */
bool ikFromAngleControls(int shoulderDeg, int elbowDeg, IktargetPosition* pos);

/**
 * @brief Convert calculated joint angles to servo PWM duties.
 * 
 * Maps the IK-derived angles to the duty values expected by the existing
 * servo_control infrastructure (0-180 degree mapping).
 * 
 * @param angles  Calculated joint angles (from ikCalculate)
 * @param duty    PWM duty values [shoulder, elbow, gripper] (0-255 range)
 */
void ikAnglesToServoDuty(const IKJointAngles* angles, int duty[3]);

#endif // ROBOREACH_IK_H