#include "servo_control.h"

static ArmJointPositions currentPositions = {
    WAIST_HOME_DEG,
    SHOULDER_HOME_DEG,
    ELBOW_HOME_DEG,
    WRIST_HOME_DEG,
    GRIPPER_HOME_DEG,
    false
};

void servoControlInit() {
    Serial.println(F("[SERVO] Initializing 5-DOF Arm Servo Subsystem..."));

    // Check if pins have been assigned by electrical team
    if (PIN_SERVO_WAIST == -1    || PIN_SERVO_SHOULDER == -1 ||
        PIN_SERVO_ELBOW == -1    || PIN_SERVO_WRIST == -1    ||
        PIN_SERVO_GRIPPER == -1) {
        currentPositions.hardwareConfigured = false;
        Serial.println(F("[SERVO] NOTICE: Servo GPIO pins are marked TBD (-1)."));
        Serial.println(F("[SERVO] Servo PWM generation is INACTIVE until wiring is confirmed."));
        return;
    }

    // Hardware attachment placeholder (to be enabled once pins are confirmed)
    // ledcSetup(SERVO_PWM_CHANNEL_WAIST, SERVO_PWM_FREQ_HZ, SERVO_PWM_RESOLUTION_BITS);
    // ledcAttachPin(PIN_SERVO_WAIST, SERVO_PWM_CHANNEL_WAIST);
    // ...
    currentPositions.hardwareConfigured = true;
    Serial.println(F("[SERVO] Servo pins configured."));
}

void servoControlUpdate(int waist, int shoulder, int elbow, int wrist, int gripper) {
    // Clamp each incoming target to safe mechanical boundaries defined in config.h
    currentPositions.waist    = constrain(waist,    WAIST_MIN_DEG,    WAIST_MAX_DEG);
    currentPositions.shoulder = constrain(shoulder, SHOULDER_MIN_DEG, SHOULDER_MAX_DEG);
    currentPositions.elbow    = constrain(elbow,    ELBOW_MIN_DEG,    ELBOW_MAX_DEG);
    currentPositions.wrist    = constrain(wrist,    WRIST_MIN_DEG,    WRIST_MAX_DEG);
    currentPositions.gripper  = constrain(gripper,  GRIPPER_OPEN_DEG, GRIPPER_CLOSED_DEG);

    // IMPORTANT:
    // When hardwareConfigured is false, NO GPIO pins are driven.
    // When the electrical team confirms pins and servo rail wiring,
    // the actual angle-to-pulse-width conversion and LEDC duty writes
    // will be executed here.
}

void servoControlPark() {
    servoControlUpdate(
        WAIST_HOME_DEG,
        SHOULDER_HOME_DEG,
        ELBOW_HOME_DEG,
        WRIST_HOME_DEG,
        GRIPPER_HOME_DEG
    );
}

ArmJointPositions getArmPositions() {
    return currentPositions;
}
