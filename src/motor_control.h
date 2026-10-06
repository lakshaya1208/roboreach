#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "../include/config.h"

/**
 * Motor Target State Representation
 */
struct MotorTargetState {
    int  leftTargetDuty;   // -255 (full reverse) to +255 (full forward)
    int  rightTargetDuty;  // -255 (full reverse) to +255 (full forward)
    bool isStopped;
    bool hardwareConfigured;
};

/**
 * Initializes the motor control subsystem.
 * Inspects pin configuration in config.h. If pins are unassigned (-1 / TBD),
 * hardware control is disabled and a warning is logged.
 */
void motorControlInit();

/**
 * Computes differential drive target duties from raw joystick inputs.
 * @param throttle Forward/backward command (-255 to +255)
 * @param steer    Left/right command (-255 to +255)
 *
 * NOTE: Does NOT write to any GPIO pins while pins are marked TBD.
 */
void motorControlUpdate(int throttle, int steer);

/**
 * Commands immediate stop on both motor channels.
 */
void motorControlStop();

/**
 * Returns the current logical motor target state.
 */
MotorTargetState getMotorState();

#endif // MOTOR_CONTROL_H
