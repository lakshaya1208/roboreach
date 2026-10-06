#include "ik.h"
#include <math.h>

/*----------------------------------------------------------------------------
 * Global configuration - set via ikInit(). These are placeholder defaults
 * that MUST be replaced with calibrated values from the physical arm.
 *----------------------------------------------------------------------------*/
static float ikShoulderToElbow = 150.0f;   // Shoulder to elbow (mm) - default placeholder
static float ikElbowToGripper    = 120.0f;  // Elbow to gripper (mm)    - default placeholder

/*----------------------------------------------------------------------------
 * ikInit - Configure IK solver with arm geometry.
 * Provide calibrated link lengths from physical arm measurement.
 *----------------------------------------------------------------------------*/
IKStatus ikInit(float shoulderToElbow, float elbowToGripper) {
    if (shoulderToElbow <= 0 || elbowToGripper <= 0) {
        return IK_INVALID_ARG;
    }

    ikShoulderToElbow = shoulderToElbow;
    ikElbowToGripper  = elbowToGripper;
    return IK_SUCCESS;
}

/*----------------------------------------------------------------------------
 * ikCalculate - Core inverse kinematics solver for 2-DOF arm.
 *
 * Given target elbow position (x, z) in the vertical plane relative to
 * shoulder base, calculate shoulder and elbow angles.
 *
 * Assumptions:
 *   - Shoulder at base origin (0,0) after waist rotation
 *   - Arm operates in vertical plane (XZ plane)
 *   - Shoulder link length: ikShoulderToElbow
 *   - Elbow/gripper link length: ikElbowToGripper
 *   - Target is reachable if distance d satisfies: |L1-L2| <= d <= L1+L2
 *   - Returns primary (elbow "down") solution
 *
 * Input:  pos->x = forward distance from shoulder (mm)
 *        pos->z = up distance from shoulder (mm) -- positive = up
 *
 * Output: angles->shoulder = shoulder pitch angle (degrees)
 *         angles->elbow    = elbow bend angle (degrees)
 *         pose->elbowPos   = computed elbow position
 *----------------------------------------------------------------------------*/
IKStatus ikCalculate(const IktargetPosition* pos, IKJointAngles* angles, IKPose* pose) {
    if (pos == NULL || angles == NULL) {
        return IK_INVALID_ARG;
    }

    const float L1 = ikShoulderToElbow;   // shoulder-to-elbow
    const float L2 = ikElbowToGripper;    // elbow-to-gripper

    // Distance from shoulder to target
    float d2 = pos->x * pos->x + pos->z * pos->z;
    float d  = sqrtf(d2);

    // Check reachability: |L1-L2| <= d <= L1+L2
    float minReach = fabsf(L1 - L2);
    float maxReach = L1 + L2;

    if (d < minReach || d > maxReach) {
        return IK_OUT_OF_REACH;
    }

    // Elbow angle via law of cosines:
    // cos(E) = (L1^2 + L2^2 - d^2) / (2 * L1 * L2)
    float cosE = (L1 * L1 + L2 * L2 - d * d) / (2.0f * L1 * L2);

    // Clamp to [-1, 1] to avoid nan from floating point
    if (cosE > 1.0f) cosE = 1.0f;
    if (cosE < -1.0f) cosE = -1.0f;

    float elbow = acosf(cosE) * 180.0f / M_PI;  // elbow bend angle (0=straight)

    // Shoulder angle:
    // Angle from vertical to line shoulder->target: phi = atan2(x, z)
    float phi = atan2f(pos->x, pos->z) * 180.0f / M_PI;

    // Shoulder offset: delta = acos((L1^2 + d^2 - L2^2) / (2 * L1 * d))
    float cosDelta = (L1 * L1 + d * d - L2 * L2) / (2.0f * L1 * d);
    if (cosDelta > 1.0f) cosDelta = 1.0f;
    if (cosDelta < -1.0f) cosDelta = -1.0f;
    float delta = acosf(cosDelta) * 180.0f / M_PI;

    // Primary solution: shoulder = phi - delta (elbow bends "down")
    float shoulder = phi - delta;

    // Clamp to reasonable servo limits (0-180)
    if (shoulder < 0.0f) shoulder = 0.0f;
    if (shoulder > 180.0f) shoulder = 180.0f;
    if (elbow < 0.0f) elbow = 0.0f;
    if (elbow > 180.0f) elbow = 180.0f;

    angles->shoulder = shoulder;
    angles->elbow    = elbow;
    angles->gripper  = 0.0f;  // Gripper angle set separately

    // Optional: compute elbow position for reporting
    if (pose != NULL) {
        // Elbow position = shoulder + upper arm vector
        // Upper arm direction = shoulder angle from vertical
        float elbowX = L1 * sinf(shoulder * M_PI / 180.0f);
        float elbowZ = L1 * cosf(shoulder * M_PI / 180.0f);

        pose->elbowPos.x = elbowX;
        pose->elbowPos.z = elbowZ;
    }

    return IK_SUCCESS;
}

/*----------------------------------------------------------------------------
 * ikFromAngleControls - Map RoboLink control values to IK target.
 *
 * Converts the RoboLink control values (arm_x for shoulder, arm_y for elbow)
 * into a elbow target position that the 2-DOF IK can use.
 * 
 * The RoboLink app provides values 0-180 for each joint. This function
 * computes where the elbow wrist end up given those angle settings.
 * 
 * For simple mapping, we treat the angle settings as desired positions
 * and compute the resulting elbow position using the current link lengths.
 *----------------------------------------------------------------------------*/
bool ikFromAngleControls(int shoulderDeg, int elbowDeg, IktargetPosition* pos) {
    if (pos == NULL) return false;

    const float L1 = ikShoulderToElbow;
    const float L2 = ikElbowToGripper;

    // Normalize shoulder angle (0-180 -> -90 to +90 from vertical, simplified)
    // Actually RoboLink 0-180 maps directly to servo angle, so:
    // shoulderDeg 0 = arm fully down, 180 = arm fully up
    // We compute the elbow position directly from the shoulder angle
    
    // Elbow position from shoulder angle
    float shoulderRad = shoulderDeg * M_PI / 180.0f;
    
    // Elbow XZ position: from shoulder at origin
    float elbowX = L1 * sinf(shoulderRad);
    float elbowZ = L1 * cosf(shoulderRad);

    pos->x = elbowX;
    pos->z = elbowZ;

    return true;
}

/*----------------------------------------------------------------------------
 * ikAnglesToServoDuty - Convert calculated joint angles to servo PWM duties.
 *
 * Maps the IK-derived angles (0-180 degree range) to the duty values
 * expected by the existing servo_control infrastructure.
 * The existing servo_control expects angles 0-180 mapped to PWM 0-255,
 * but since we're using 16-bit resolution, we map directly.
 *----------------------------------------------------------------------------*/
void ikAnglesToServoDuty(const IKJointAngles* angles, int duty[3]) {
    if (angles == NULL || duty == NULL) return;

    // Map angles (0-180) directly to duty 0-255 for compatibility
    // with existing servo_control clamping logic
    duty[0] = (int)(angles->shoulder * 255.0f / 180.0f);   // Shoulder duty
    duty[1] = (int)(angles->elbow    * 255.0f / 180.0f);   // Elbow duty
    duty[2] = (int)(angles->gripper  * 255.0f / 180.0f);   // Gripper duty
}