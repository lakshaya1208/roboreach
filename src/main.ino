/*
 * ============================================================================
 * RoboReach Robot Main Controller Firmware (Architecture Skeleton)
 * ============================================================================
 * File: src/main.ino
 * Authoritative Hardware Source: "ROBOREACH BOM - Sheet1.pdf"
 * 
 * ARCHITECTURE OVERVIEW:
 * RoboLink Mobile App (Wi-Fi / UDP)
 *        ↓
 *      ESP32
 *     ↙     ↘
 *  BTS7960   5-DOF Servo Control
 *    ↓         ↓
 * 2 DC Motors  Waist (DS3225 25kg)
 * (12V 100RPM) Shoulder (Annimos 60kg)
 *              Elbow (DS3225 25kg)
 *              Wrist (DS3225 25kg)
 *              End Effector (MG90 Metal)
 *
 * SAFETY NOTICE:
 * Hardware pin assignments are currently TBD pending wiring confirmation
 * from the electrical team. All physical GPIO writes are gated off in
 * motor_control and servo_control. This skeleton safely handles network
 * communication, logical coordinate mapping, and safety timeouts.
 * ============================================================================
 */

#include <Arduino.h>
#include "../include/config.h"
#include "robolink_handler.h"
#include "motor_control.h"
#include "servo_control.h"

static unsigned long lastTelemetryPrint = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println(F("\n========================================================"));
    Serial.println(F("         RoboReach: ESP32 Robot Controller               "));
    Serial.println(F("========================================================"));
    Serial.println(F("Authoritative Hardware: Final BOM (ROBOREACH BOM - Sheet1.pdf)"));
    Serial.println(F("Status: Software Architecture Ready (Hardware Pins TBD)"));
    Serial.println(F("--------------------------------------------------------"));

    // 1. Initialize Motor Control Subsystem (checks for TBD pins)
    motorControlInit();

    // 2. Initialize Servo Control Subsystem (checks for TBD pins)
    servoControlInit();

    // 3. Initialize RoboLink Wireless Communication
    robolinkHandlerInit();

    Serial.println(F("========================================================"));
    Serial.println(F("System initialized in SAFE ARCHITECTURE MODE."));
    Serial.println(F("Awaiting connection from RoboLink app...\n"));
}

void loop() {
    // 1. Process incoming network packets (non-blocking)
    robolinkHandlerUpdate();

    // 2. Dispatch logical control inputs if link is active
    if (robolinkIsConnected()) {
        // Base Drive commands
        int throttle = robolinkGetThrottle();
        int steer    = robolinkGetSteer();
        motorControlUpdate(throttle, steer);

        // 5-DOF Arm commands
        int waist    = robolinkGetWaist();
        int shoulder = robolinkGetShoulder();
        int elbow    = robolinkGetElbow();
        int wrist    = robolinkGetWrist();
        int gripper  = robolinkGetGripper();
        servoControlUpdate(waist, shoulder, elbow, wrist, gripper);
    } else {
        // Safety Failsafe: stop mobile base when connection is lost
        motorControlStop();
    }

    // 3. Periodic Diagnostic Log (Every 2 seconds, non-blocking)
    if (millis() - lastTelemetryPrint >= 2000) {
        lastTelemetryPrint = millis();

        if (!robolinkIsConnected()) {
            Serial.println(F("[STATUS] No active RoboLink stream. Base motors STOPPED (Failsafe)."));
        } else {
            MotorTargetState mState = getMotorState();
            ArmJointPositions aPos  = getArmPositions();

            Serial.println(F("--- [ROBOREACH ACTIVE CONTROL STATE] ---"));
            Serial.printf("  Base Target : Left=%d, Right=%d (Stopped=%s)\n",
                          mState.leftTargetDuty, mState.rightTargetDuty,
                          mState.isStopped ? "YES" : "NO");
            Serial.printf("  Arm Angles  : Waist=%d°, Shoulder=%d°, Elbow=%d°, Wrist=%d°, Gripper=%d°\n",
                          aPos.waist, aPos.shoulder, aPos.elbow, aPos.wrist, aPos.gripper);
            Serial.printf("  Hardware Out: %s (Gated until wiring confirmed)\n",
                          (mState.hardwareConfigured && aPos.hardwareConfigured) ? "ENABLED" : "SAFE / INACTIVE");
            Serial.println(F("----------------------------------------"));
        }
    }
}
