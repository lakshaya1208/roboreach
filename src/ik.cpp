#include "ik.h"
#include <math.h>

/*----------------------------------------------------------------------------
 * Default configuration - placeholder link lengths.
 * MUST be calibrated to actual physical arm measurements!
 *----------------------------------------------------------------------------*/
static IKConfig defaultConfig = {
    .shoulderToElbow   = 150.0f,   // Shoulder to elbow link length (mm)
    .elbowToWrist      = 150.0f,   // Elbow to wrist/link length (mm)
    .wristToGripper    = 50.0f,    // Wrist to gripper center (mm)
    .shoulderZOffset   = 50.0f,    // Z height of shoulder pivot (mm)
    .baseRadius        = 0.0f,     // Base center to shoulder pivot (mm)
    .waistMin          = 0.0f,
    .waistMax          = 360.0f,
    .shoulderMin       = 0.0f,
    .shoulderMax       = 180.0f,
    .elbowMin          = 0.0f,
    .elbowMax          = 180.0f,
    .wristMin          = -90.0f,
    .wristMax          = 90.0f
};

/*----------------------------------------------------------------------------
 * ikInit - Configure IK solver with arm geometry.
 * Provide custom link lengths after physical calibration.
 * If config is NULL, uses default placeholder values.
 *----------------------------------------------------------------------------*/
IKStatus ikInit(const IKConfig* config) {
    if (config == NULL) {
        // Use defaults; caller should replace with calibrated values
        return IK_SUCCESS;
    }

    // Validate angle limits
    if (config->shoulderMin >= config->shoulderMax ||
        config->elbowMin >= config->elbowMax) {
        return IK_INVALID_ARG;
    }

    // Copy provided config
    defaultConfig.shoulderToElbow   = config->shoulderToElbow;
    defaultConfig.elbowToWrist      = config->elbowToWrist;
    defaultConfig.wristToGripper    = config->wristToGripper;
    defaultConfig.shoulderZOffset   = config->shoulderZOffset;
    defaultConfig.baseRadius        = config->baseRadius;
    defaultConfig.waistMin          = config->waistMin;
    defaultConfig.waistMax          = config->waistMax;
    defaultConfig.shoulderMin       = config->shoulderMin;
    defaultConfig.shoulderMax       = config->shoulderMax;
    defaultConfig.elbowMin          = config->elbowMin;
    defaultConfig.elbowMax          = config->elbowMax;

    return IK_SUCCESS;
}

/*----------------------------------------------------------------------------
 * ikCalculate2DOF - 2-DOF inverse kinematics for shoulder + elbow.
 *
 * Given target wrist position (x, z) in the vertical plane relative to
 * base origin, calculate shoulder and elbow angles.
 *
 * Assumptions:
 *   - Shoulder at base origin (0,0,0) after waist rotation
 *   - Arm operates in vertical plane (XZ plane)
 *   - Shoulder link length: shoulderToElbow
 *   - Elbow/wrist link length: elbowToWrist
 *   - Target is reachable if distance d satisfies: |L1-L2| <= d <= L1+L2
 *   - Returns primary (elbow "down") solution
 *
 * Input:  pos->x = forward distance from shoulder (mm)
 *        pos->z = up distance from shoulder (mm) -- positive = up
 *        pos->y is ignored for 2-DOF planar arm
 *
 * Output: angles->shoulder = shoulder pitch angle (degrees)
 *         angles->elbow    = elbow bend angle (degrees)
 *         angles->waist    = unchanged (caller sets prior)
 *----------------------------------------------------------------------------*/
IKStatus ikCalculate2DOF(const IktargetPosition* pos, IKJointAngles* angles, IKPose* pose) {
    if (pos == NULL || angles == NULL) {
        return IK_INVALID_ARG;
    }

    // Use default config
    const IKConfig& cfg = defaultConfig;

    // Distance from shoulder to target in XZ plane
    float d2 = pos->x * pos->x + pos->z * pos->z;
    float d = sqrtf(d2);

    // Check reachability: |L1-L2| <= d <= L1+L2
    float L1 = cfg.shoulderToElbow;   // shoulder-to-elbow
    float L2 = cfg.elbowToWrist;      // elbow-to-wrist
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
    // Shoulder offset: delta = acos((L1^2 + d^2 - L2^2) / (2 * L1 * d))
    float phi = atan2f(pos->x, pos->z) * 180.0f / M_PI;

    float cosDelta = (L1 * L1 + d * d - L2 * L2) / (2.0f * L1 * d);
    if (cosDelta > 1.0f) cosDelta = 1.0f;
    if (cosDelta < -1.0f) cosDelta = -1.0f;
    float delta = acosf(cosDelta) * 180.0f / M_PI;

    // Primary solution: shoulder = phi - delta (elbow bends "down")
    float shoulder = phi - delta;

    // Clamp to configured limits
    if (shoulder < cfg.shoulderMin) shoulder = cfg.shoulderMin;
    if (shoulder > cfg.shoulderMax) shoulder = cfg.shoulderMax;
    if (elbow < cfg.elbowMin) elbow = cfg.elbowMin;
    if (elbow > cfg.elbowMax) elbow = cfg.elbowMax;

    angles->shoulder = shoulder;
    angles->elbow    = elbow;
    // Waist unchanged - caller should set based on (x,y) projection

    // Optional: compute wrist position for reporting
    if (pose != NULL) {
        // Wrist position = elbow position + forearm vector
        // Elbow position = shoulder + upper arm vector
        float shoulderX = 0.0f;  // after waist rotation applied by caller
        float shoulderZ = cfg.shoulderZOffset;

        // Upper arm end (elbow) position:
        float elbowX = shoulderX + L1 * sinf(shoulder * M_PI / 180.0f);
        float elbowZ = shoulderZ + L1 * cosf(shoulder * M_PI / 180.0f);

        // Wrist position: elbow + forearm aligned with shoulder angle + elbow angle
        // Actually forearm direction = shoulder angle + elbow angle (relative to vertical)
        float wristX = elbowX + L2 * sinf((shoulder + elbow) * M_PI / 180.0f);
        float wristZ = elbowZ + L2 * cosf((shoulder + elbow) * M_PI / 180.0f);

        pose->wristPos.x = wristX;
        pose->wristPos.z = wristZ;
        pose->wristPos.y = 0.0f;  // planar arm ignores y
    }

    return IK_SUCCESS;
}

/*----------------------------------------------------------------------------
 * ikFromAngleControls - Map existing angle-based controls to IK target.
 *
 * Converts the current RoboLink angle inputs (0-180 per joint) into a
 * wrist target position that the 2-DOF IK can use. Useful for gradual
 * transition from direct angle control to position-based control.
 *----------------------------------------------------------------------------*/
bool ikFromAngleControls(
    int waistDeg, int shoulderDeg, int elbowDeg, int wristDeg, int gripperDeg,
    IktargetPosition* pos, const IKConfig* config
) {
    if (pos == NULL) return false;

    const IKConfig& cfg = (config != NULL) ? *config : defaultConfig;

    // For simple mapping, treat the shoulder/elbow/wrist angles as desired
    // positions and compute a target wrist position.
    // This is a inverse mapping: given joint angles, compute where wrist ends up.

    // Assume waist=0 for this mapping (caller should handle waist rotation)
    float waistRad = 0.0f;

    // Shoulder and elbow as fractions of their ranges
    float shoulderNorm = (float)shoulderDeg / 180.0f;  // 0-1
    float elbowNorm    = (float)elbowDeg    / 180.0f;

    // Compute shoulder Z position (simplified: assume shoulder at fixed height)
    float shoulderZ = cfg.shoulderZOffset;

    // Compute elbow position based on shoulder angle and elbow bend
    // Simplified: treat as 2-Link planar arm with given angles
    float L1 = cfg.shoulderToElbow;
    float L2 = cfg.elbowToWrist;

    // Elbow position
    float elbowX = L1 * sinf(shoulderDeg * M_PI / 180.0f);
    float elbowZ = shoulderZ + L1 * cosf(shoulderDeg * M_PI / 180.0f);

    // Wrist position = elbow + forearm
    // Forearm direction depends on elbow angle
    float forearmAngle = shoulderDeg + elbowDeg;  // simplified additive model
    float wristX = elbowX + L2 * sinf(forearmAngle * M_PI / 180.0f);
    float wristZ = elbowZ + L2 * cosf(forearmAngle * M_PI / 180.0f);

    pos->x = wristX;
    pos->z = wristZ;
    pos->y = 0.0f;

    return true;
}

/*----------------------------------------------------------------------------
 * ikAnglesToServoDuty - Convert calculated joint angles to servo PWM duties.
 *
 * Maps the IK-derived angles (0-180 degree range) to the duty values
 * expected by the existing servo_control infrastructure.
 *----------------------------------------------------------------------------*/
void ikAnglesToServoDuty(const IKJointAngles* angles, int duty[5]) {
    if (angles == NULL || duty == NULL) return;

    // Map angles (0-180) directly to PWM duty 0-180 for compatibility
    // with existing servo_control clamping logic
    duty[0] = (int)angles->waist;       // Waist
    duty[1] = (int)angles->shoulder;    // Shoulder
    duty[2] = (int)angles->elbow;       // Elbow
    duty[3] = (int)angles->wrist;       // Wrist
    duty[4] = (int)angles->gripper;     // Gripper
}