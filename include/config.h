#ifndef ROBOREACH_CONFIG_H
#define ROBOREACH_CONFIG_H

#include <Arduino.h>

/**
 * ============================================================================
 * RoboReach Firmware Configuration & Hardware Placeholders
 * ============================================================================
 * Authoritative Hardware Source: "ROBOREACH BOM - Sheet1.pdf"
 * 
 * IMPORTANT NOTICE:
 * All pin assignments, electrical parameters, and physical calibrations below
 * are set per the 2-DOF arm specification. 
 * 
 * Pins are assigned - no more TBD placeholders for GPIO outputs.
 * ============================================================================
 */

 /* ----------------------------------------------------------------------------
  * 1. Microcontroller Target
  * ----------------------------------------------------------------------------
  * Microcontroller: ESP32 (Dual-core Tensilica Xtensa LX6)
  * Board Variant:   esp32dev (Arduino-ESP32 core 3.x)
  * Note: Avoid strapping pins (GPIO 0, 2, 12, 15) and input-only pins (34-39)
  *       when assigning output controls once the board is confirmed.
  */
 #define ESP32_BOARD_VARIANT_TBD       0

/* ----------------------------------------------------------------------------
 * 2. Mobile Base Drive System (BTS7960 + 2 x 12V 100RPM Motors)
 * ----------------------------------------------------------------------------
 * Driver: High-Power BTS7960 Dual H-Bridge (43A rating)
 * Motors: 2 x 12V 100RPM Geared DC Motors
 * Wheels: 100mm Diameter
 *
 * BTS7960 Control Pins (Left & Right Channels):
 * Confirmed per task requirements:
 */
 #define PIN_MOTOR_L_RPWM              (25)  // Left Forward PWM
 #define PIN_MOTOR_L_LPWM              (26)  // Left Reverse PWM
 #define PIN_MOTOR_L_EN                (27)  // Left Driver Enable (R_EN/L_EN bridged or separate)

 #define PIN_MOTOR_R_RPWM              (27)  // Right Forward PWM
 #define PIN_MOTOR_R_LPWM              (33)  // Right Reverse PWM
 #define PIN_MOTOR_R_EN                (33)  // Right Driver Enable (shared or separate)

// Motor PWM Generation Settings (ESP32 LEDC Timer)
 #define MOTOR_PWM_FREQ_HZ             (20000) // Ultrasonic 20kHz default to avoid audible motor hum
 #define MOTOR_PWM_RESOLUTION_BITS     (8)     // 8-bit resolution (0 to 255)
 #define MOTOR_PWM_CHANNEL_L_FWD       (0)     // LEDC Channel 0
 #define MOTOR_PWM_CHANNEL_L_REV       (1)     // LEDC Channel 1
 #define MOTOR_PWM_CHANNEL_R_FWD       (2)     // LEDC Channel 2
 #define MOTOR_PWM_CHANNEL_R_REV       (3)     // LEDC Channel 3

// Base Deadband & Speed Scaling
 #define MOTOR_INPUT_DEADBAND          (10)    // Ignore minor joystick center drift (-10 to +10)
 #define MOTOR_MAX_DUTY                (255)   // Max 8-bit duty cycle

/* ----------------------------------------------------------------------------
 * 3. 2-DOF Robotic Arm (Shoulder + Elbow + Gripper)
 * ----------------------------------------------------------------------------
 * Actuator Mapping per task specification:
 * - Shoulder Joint: Annimos 60kg Digital Servo (High torque lift)
 * - Elbow Joint: 20kg Digital Servo
 * - End Effector: SG90 Micro Servo (gripper)
 * - Arm Geometry:
 *   L1 = 150 mm (shoulder to elbow)
 *   L2 = 120 mm (elbow to gripper center)
 *
 * Servo Signal GPIO Pins: Confirmed
 */
 #define PIN_SERVO_SHOULDER            (18)  // Shoulder lift joint (Annimos 60kg)
 #define PIN_SERVO_ELBOW               (19)  // Elbow joint (20kg servo)
 #define PIN_SERVO_GRIPPER             (21)  // End effector clamp / gripper (SG90)

// Servo PWM Settings (ESP32 LEDC Channels)
// Using channels 4-8 as before, but only for 3 servos now
 #define SERVO_PWM_FREQ_HZ             (50)  // Standard 50Hz RC servo refresh rate
 #define SERVO_PWM_RESOLUTION_BITS     (16)  // 16-bit resolution for smooth position control
 #define SERVO_PWM_CHANNEL_SHOULDER    (4)   // LEDC Channel 4
 #define SERVO_PWM_CHANNEL_ELBOW       (5)   // LEDC Channel 5
 #define SERVO_PWM_CHANNEL_GRIPPER     (6)   // LEDC Channel 6

/* ----------------------------------------------------------------------------
 * 4. Servo Safe Calibration Limits & Park Positions
 * ----------------------------------------------------------------------------
 * All angle boundaries (degrees) and pulse width limits (microseconds)
 * must be verified on the physical mechanical assembly to prevent binding.
 * Values below are standard starting points - calibrate on hardware!
 */
// Pulse width defaults (500us to 2500us standard for 180/270 deg digital servos)
 #define SERVO_PULSE_MIN_US            (500)
 #define SERVO_PULSE_MAX_US            (2500)

// Joint Angle Travel Limits (Degrees)
// Shoulder: 0-180 degrees (Annimos 60kg lifts arm)
// Elbow: 0-180 degrees (20kg arm extension)
// Gripper (SG90): 0-90 degrees (0=open, 90=closed)
 #define SHOULDER_MIN_DEG              (0)   // Lower physical stop
 #define SHOULDER_MAX_DEG              (180) // Upper physical stop
 #define SHOULDER_HOME_DEG             (90)  // Neutral resting angle

 #define ELBOW_MIN_DEG                 (0)   // Lower physical stop
 #define ELBOW_MAX_DEG                 (180) // Upper physical stop
 #define ELBOW_HOME_DEG                (90)  // Neutral resting angle

 #define GRIPPER_MIN_DEG               (0)   // Full open position
 #define GRIPPER_MAX_DEG               (90)  // Full closed position
 #define GRIPPER_HOME_DEG              (0)   // Open resting state

/* ----------------------------------------------------------------------------
 * 5. RoboLink Mobile Application Protocol & Control Keys
 * ----------------------------------------------------------------------------
 * Communication: UDP broadcast over Wi-Fi
 * Default IP:    192.168.4.1 (ESP32 AP Mode)
 * Default Port:  4210
 * Control mapping for 2-DOF arm:
 *   arm_x -> shoulder control (0 to 180)
 *   arm_y -> elbow control (0 to 180)
 *   grip -> gripper control (0 to 90)
 */
 #define ROBOLINK_WIFI_SSID            "RoboReach"
 #define ROBOLINK_WIFI_PASS            "roboreach123" // Minimum 8 characters
 #define ROBOLINK_UDP_PORT             (4210)
 #define ROBOLINK_TIMEOUT_MS           (500)          // Failsafe triggers if no packets for 500ms

// Documented Logical Keys from Mobile App
 #define KEY_BASE_THROTTLE             "throttle"     // Base forward (+255) / backward (-255)
 #define KEY_BASE_STEER                "steer"        // Base left (-255) / right (+255)
 #define KEY_ARM_SHOULDER              "arm_x"        // Shoulder elevation (0 to 180)
 #define KEY_ARM_ELBOW                 "arm_y"        // Elbow extension (0 to 180)
 #define KEY_ARM_GRIPPER               "grip"         // Gripper open/close (0 to 90)

/* ----------------------------------------------------------------------------
 * 6. Electrical Power Architecture Notes
 * ----------------------------------------------------------------------------
 * - Battery: TBD (Chemistry, voltage, capacity, C-rating) - to calibrate on hardware
 * - Buck Converter: TBD (Exact module, voltage, current capacity) - to calibrate on hardware
 * - Dedicated Servo Rail: TBD (Annimos 60kg high-surge current isolation) - to calibrate on hardware
 */
 #define POWER_BATTERY_CELLS_TBD       0
 #define POWER_SERVO_RAIL_VOLTAGE_TBD  0

#endif // ROBOREACH_CONFIG_H