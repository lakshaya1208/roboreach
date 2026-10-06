#ifndef ROBOREACH_IK_H
#define ROBOREACH_IK_H

#include <Arduino.h>

/**
 * 5-DOF Robotic Arm Inverse Kinematics
 * 
 * Arm layout (from base):
 * 1. Waist: Base rotation joint (yaw)
 * 2. Shoulder: Primary lift joint (pitch)
 * 3. Elbow: Forearm elevation joint (pitch)
 * 4. Wrist: Wrist pitch joint (pitch)
 * 5. Gripper: End effector clamp
 * 
 * Note: This IK solver operates in the 3D workspace of the shoulder-elbow-wrist chain,
 * with waist providing rotational orientation around the vertical axis.
 * 
 * All link lengths must be configured in config.h (IK_LINK_* below).
 * Units: millimeters for lengths, degrees for angles.
 */

// *****************************************************************************
// ***                          Configuration Constants                        ***
// *****************************************************************************

/**
 * Link lengths - MUST be calibrated to physical arm measurement.
 * These are placeholders; verify with actual printed/mechanized arm.
 */
#define IK_LINK_SHOULDER_TO_ELBOW   (150.0f)  // Distance shoulder joint to elbow joint (mm)
#define IK_LINK_ELBOW_TO_WRIST      (150.0f)  // Distance elbow joint to wrist joint (mm)
#define IK_LINK_WRIST_TO_GRIPPER    (50.0f)   // Distance wrist joint to grip center (mm)
#define IK_SHOULDER_Z_OFFSET        (50.0f)   // Z-offset from base to shoulder joint (mm)
#define IK_BASE_RADIUS              (0.0f)    // Distance from base center to shoulder pivot (mm)

// *****************************************************************************
// ***                            Data Structures                             ***
// *****************************************************************************

/**
 * 3D target position for the wrist joint (the point where wrist pivot is).
 * All coordinates in millimeters, origin at base center.
 */
struct IktargetPosition {
    float x;   // Forward (+) / Backward (-) from base (mm)
    float y;   // Left (+) / Right (-) from center (mm)  
    float z;   // Up (+) / Down (-) from base (mm)
};

/**
 * Calculated joint angles for the 5-DOF arm.
 * All angles in degrees, range depends on servo specifications.
 */
struct IKJointAngles {
    float waist;      // Base rotation, 0-360° (positive = counter-clockwise looking from above)
    float shoulder;   // Shoulder pitch, 0-180° (positive = arm lifts up)
    float elbow;      // Elbow pitch, 0-180° (positive = arm extends forward)
    float wrist;      // Wrist pitch, -90 to +90° (positive = wrist tilts up)
    float gripper;    // Gripper state, 0-90° (0=open, 90=closed)
};

/**
 * Full arm pose combining wrist position and joint angles.
 */
struct IKPose {
    IktargetPosition wristPos;  // Wrist joint position in 3D space
    IKJointAngles angles;       // Calculated joint angles
};

/**
 * IK status return code.
 */
typedef enum {
    IK_SUCCESS      = 0,   // Inverse kinematics solved successfully
    IK_OUT_OF_REACH = 1,   // Target position is outside arm workspace
    IK_SINGULARITY  = 2,   // Target near singularity (arm fully extended/folded)
    IK_INVALID_ARG  = 3    // Invalid input parameters
} IKStatus;

/**
 * IK configuration - adjustable parameters for different arm geometries.
 */
struct IKConfig {
    // Link lengths (in mm, must match physical arm)
    float shoulderToElbow;     // Shoulder joint to elbow joint
    float elbowToWrist;        // Elbow joint to wrist joint
    float wristToGripper;      // Wrist joint to gripper center
    float shoulderZOffset;     // Z offset from base to shoulder joint
    float baseRadius;          // Distance from base center to shoulder pivot
    
    // Angle limits (in degrees)
    float waistMin;            // Minimum waist angle
    float waistMax;            // Maximum waist angle
    float shoulderMin;         // Minimum shoulder angle
    float shoulderMax;         // Maximum shoulder angle
    float elbowMin;            // Minimum elbow angle
    float elbowMax;            // Maximum elbow angle
    float wristMin;            // Minimum wrist angle
    float wristMax;            // Maximum wrist angle
};

/**
 * @brief Initialize IK solver with custom configuration.
 * @param config Pointer to IKConfig with link lengths and angle limits.
 * @return IKStatus IK_SUCCESS if config is valid, IK_INVALID_ARG otherwise.
 */
IKStatus ikInit(const IKConfig* config);

/**
 * @brief Calculate inverse kinematics for 5-DOF arm.
 * 
 * Given a target wrist position (x, y, z), calculates the joint angles
 * needed to reach that position. The waist angle is calculated based on
 * the (x, y) projection; shoulder/elbow/wrist solve the vertical plane.
 * 
 * @param[in]  pos   Target wrist position in 3D space (mm)
 * @[out]      angles  Calculated joint angles (output)
 * @param[out] pose    Full pose including wrist position and angles (output)
 * @return IKStatus Status of the calculation.
 */
IKStatus ikCalculate(const IktargetPosition* pos, IKJointAngles* angles, IKPose* pose);

/**
 * @brief Convert RoboLink control values (0-180° joint commands) to IK target.
 * 
 * This helper maps the existing angle-based control scheme to wrist positions.
 * Useful for gradual transition from direct angle control to position-based control.
 * 
 * @param waistDeg      Waist angle from RoboLink (0-180)
 * @param shoulderDeg   Shoulder angle from RoboLink (0-180)
 * @param elbowDeg      Elbow angle from RoboLink (0-180)
 * @param wristDeg      Wrist angle from RoboLink (0-180)
 * @param gripperDeg    Gripper angle from RoboLink (0-90)
 * @param[out] pos      Computed wrist target position (output)
 * @param[out] config   IK configuration to use
 * @return true if calculation valid, false if out of workspace.
 */
bool ikFromAngleControls(
    int waistDeg, int shoulderDeg, int elbowDeg, int wristDeg, int gripperDeg,
    IktargetPosition* pos, const IKConfig* config
);

/**
 * @brief Default IK configuration with placeholder link lengths.
 * Update these values after physical arm calibration.
 */
extern const IKConfig IK_DEFAULT_CONFIG;

/**
 * @brief Convert wrist position to duty cycles for servo PWM.
 * 
 * Takes calculated joint angles and maps them to the ESP32 LEDC PWM duty values
 * expected by the existing servo_control infrastructure.
 * 
 * @param angles  Calculated joint angles (from ikCalculate)
 * @return int[5] PWM duty values [waist, shoulder, elbow, wrist, gripper] (0-180 degree mapping)
 */
void ikAnglesToServoDuty(const IKJointAngles* angles, int duty[5]);

#endif // ROBOREACH_IK_H