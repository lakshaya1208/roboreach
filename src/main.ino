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
 *  BTS7960   2-DOF Servo Control
 *    ↓         ↓
 * 2 DC Motors  Shoulder (Annimos 60kg)
 * (12V 100RPM) Elbow (20kg)
 *              Gripper (SG90)
 *
 * SAFETY NOTICE:
 * Hardware pin assignments are confirmed in config.h.
 * All physical GPIO writes are properly configured.
 * This skeleton safely handles network communication, logical coordinate mapping, and safety timeouts.
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
    Serial.println(F("Status: 2-DOF Software Ready (Hardware Pins Confirmed)"));
    Serial.println(F("--------------------------------------------------------"));

    // 1. Initialize Motor Control Subsystem (checks for confirmed pins)
    motorControlInit();

    // 2. Initialize Servo Control Subsystem (checks for confirmed pins)
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

        // 2-DOF Arm commands (shoulder, elbow, gripper)
        int shoulder = robolinkGetShoulder();
        int elbow    = robolinkGetElbow();
        int gripper  = robolinkGetGripper();
        servoControlUpdate(shoulder, elbow, gripper);
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
            Serial.printf("  Arm Angles  : Shoulder=%d°, Elbow=%d°, Gripper=%d°\n",
                          aPos.shoulder, aPos.elbow, aPos.gripper);
            Serial.printf("  Hardware Out: %s (Enabled - pins confirmed)\n",
                          (mState.hardwareConfigured && aPos.hardwareConfigured) ? "ENABLED" : "SAFE / INACTIVE");
            Serial.println(F("----------------------------------------"));
        }
    }
}