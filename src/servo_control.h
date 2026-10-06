#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include "../include/config.h"

/**
 * 5-DOF Robotic Arm Joint Target Angles (Degrees)
 */
struct ArmJointPositions {
    int  waist;      // Base yaw rotation (degrees)
    int  shoulder;   // Primary lift joint (Annimos 60kg, degrees)
    int  elbow;      // Forearm elevation joint (degrees)
    int  wrist;      // Wrist tilt joint (degrees)
    int  gripper;    // End effector opening (degrees: 0=open, 90=closed)
    bool hardwareConfigured;
};

/**
 * Initializes the 5-DOF arm servo control subsystem.
 * Checks pin configuration in config.h. If pins are unassigned (-1 / TBD),
 * hardware control is disabled and a warning is logged.
 */
void servoControlInit();

/**
 * Updates target joint angles, clamping to safe mechanical boundaries.
 * 
 * NOTE: Does NOT write to any GPIO pins or LEDC channels while pins are TBD.
 */
void servoControlUpdate(int waist, int shoulder, int elbow, int wrist, int gripper);

/**
 * Commands arm joints to their safe home/park positions.
 */
void servoControlPark();

/**
 * Returns current target positions of the 5 joints.
 */
ArmJointPositions getArmPositions();

#endif // SERVO_CONTROL_H
