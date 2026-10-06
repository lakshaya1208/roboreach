#include "motor_control.h"

static MotorTargetState currentState = {0, 0, true, false};

void motorControlInit() {
    Serial.println(F("[MOTOR] Initializing Motor Control Subsystem..."));

    // Check if pins have been assigned by electrical team
    if (PIN_MOTOR_L_RPWM == -1 || PIN_MOTOR_L_LPWM == -1 ||
        PIN_MOTOR_R_RPWM == -1 || PIN_MOTOR_R_LPWM == -1) {
        currentState.hardwareConfigured = false;
        Serial.println(F("[MOTOR] NOTICE: BTS7960 GPIO pins are marked TBD (-1)."));
        Serial.println(F("[MOTOR] Hardware GPIO control is INACTIVE until wiring is confirmed."));
        return;
    }

    // Hardware initialization placeholder (to be enabled once pins are confirmed)
    // ledcSetup(MOTOR_PWM_CHANNEL_L_FWD, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RESOLUTION_BITS);
    // ledcAttachPin(PIN_MOTOR_L_RPWM, MOTOR_PWM_CHANNEL_L_FWD);
    // ...
    currentState.hardwareConfigured = true;
    Serial.println(F("[MOTOR] Motor pins configured."));
}

void motorControlUpdate(int throttle, int steer) {
    // Apply deadband around center position
    if (abs(throttle) < MOTOR_INPUT_DEADBAND) {
        throttle = 0;
    }
    if (abs(steer) < MOTOR_INPUT_DEADBAND) {
        steer = 0;
    }

    // Differential Drive Mixing:
    // Left  = Throttle + Steer
    // Right = Throttle - Steer
    int leftRaw  = throttle + steer;
    int rightRaw = throttle - steer;

    // Constrain to allowable PWM range [-MOTOR_MAX_DUTY, +MOTOR_MAX_DUTY]
    currentState.leftTargetDuty  = constrain(leftRaw,  -MOTOR_MAX_DUTY, MOTOR_MAX_DUTY);
    currentState.rightTargetDuty = constrain(rightRaw, -MOTOR_MAX_DUTY, MOTOR_MAX_DUTY);

    if (currentState.leftTargetDuty == 0 && currentState.rightTargetDuty == 0) {
        currentState.isStopped = true;
    } else {
        currentState.isStopped = false;
    }

    // IMPORTANT:
    // When hardwareConfigured is false, NO GPIO pins are driven.
    // When the electrical team confirms pins, the actual LEDC duty writes
    // will be executed here.
}

void motorControlStop() {
    currentState.leftTargetDuty  = 0;
    currentState.rightTargetDuty = 0;
    currentState.isStopped       = true;

    // Safe stop (no GPIO writes while hardwareConfigured is false)
}

MotorTargetState getMotorState() {
    return currentState;
}
