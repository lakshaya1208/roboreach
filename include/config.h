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
 * are currently TBD (To Be Determined) pending confirmation from the
 * electrical team. 
 *
 * DO NOT assign random or prototype pins here. Pins set to -1 indicate
 * unassigned placeholders to guarantee safety against accidental hardware drive.
 * ============================================================================
 */

/* ----------------------------------------------------------------------------
 * 1. Microcontroller Target
 * ----------------------------------------------------------------------------
 * Microcontroller: ESP32 (Dual-core Tensilica Xtensa LX6)
 * Board Variant:   TBD (e.g. NodeMCU-32S, ESP32 DevKit V1, ESP32-WROOM-32)
 * Note: Avoid strapping pins (GPIO 0, 2, 12, 15) and input-only pins (34-39)
 *       when assigning output controls once the board is confirmed.
 */
#define ESP32_BOARD_VARIANT_TBD       1

/* ----------------------------------------------------------------------------
 * 2. Mobile Base Drive System (BTS7960 + 2 x 12V 100RPM Motors)
 * ----------------------------------------------------------------------------
 * Driver: High-Power BTS7960 Dual H-Bridge (43A rating)
 * Motors: 2 x 12V 100RPM Geared DC Motors
 * Wheels: 100mm Diameter
 *
 * BTS7960 Control Pins (Left & Right Channels):
 * Set to -1 until confirmed by the electrical team.
 */
#define PIN_MOTOR_L_RPWM              (-1)  // TBD: Left Forward PWM
#define PIN_MOTOR_L_LPWM              (-1)  // TBD: Left Reverse PWM
#define PIN_MOTOR_L_EN                (-1)  // TBD: Left Driver Enable (R_EN/L_EN bridged or separate)

#define PIN_MOTOR_R_RPWM              (-1)  // TBD: Right Forward PWM
#define PIN_MOTOR_R_LPWM              (-1)  // TBD: Right Reverse PWM
#define PIN_MOTOR_R_EN                (-1)  // TBD: Right Driver Enable (R_EN/L_EN bridged or separate)

// Motor PWM Generation Settings (ESP32 LEDC Timer)
#define MOTOR_PWM_FREQ_HZ             (20000) // Ultrasonic 20kHz default to avoid audible motor hum (TBD)
#define MOTOR_PWM_RESOLUTION_BITS     (8)     // 8-bit resolution (0 to 255)
#define MOTOR_PWM_CHANNEL_L_FWD       (0)     // LEDC Channel 0
#define MOTOR_PWM_CHANNEL_L_REV       (1)     // LEDC Channel 1
#define MOTOR_PWM_CHANNEL_R_FWD       (2)     // LEDC Channel 2
#define MOTOR_PWM_CHANNEL_R_REV       (3)     // LEDC Channel 3

// Base Deadband & Speed Scaling
#define MOTOR_INPUT_DEADBAND          (10)    // Ignore minor joystick center drift (-10 to +10)
#define MOTOR_MAX_DUTY                (255)   // Max 8-bit duty cycle

/* ----------------------------------------------------------------------------
 * 3. 5-DOF Articulated Robotic Arm (Actuators & Servos)
 * ----------------------------------------------------------------------------
 * Actuator Mapping from Final BOM:
 * - Waist Joint:        DS3225 (25kg digital servo)
 * - Shoulder Joint:     Annimos 60kg Digital Servo (High torque lift)
 * - Elbow Joint:        DS3225 (25kg digital servo)
 * - Wrist Joint:        DS3225 (25kg digital servo)
 * - End Effector:       MG90 Metal Gear Micro Servo
 * - Transmissions:      3 x GT2 Timing Belts (6mm width, 2mm pitch)
 *
 * Servo Signal GPIO Pins:
 * Set to -1 until confirmed by the electrical team.
 */
#define PIN_SERVO_WAIST               (-1)  // TBD: Base rotation joint
#define PIN_SERVO_SHOULDER            (-1)  // TBD: Shoulder lift joint (Annimos 60kg)
#define PIN_SERVO_ELBOW               (-1)  // TBD: Forearm lift joint
#define PIN_SERVO_WRIST               (-1)  // TBD: Wrist tilt joint
#define PIN_SERVO_GRIPPER             (-1)  // TBD: End effector clamp / gripper

// Servo PWM Settings (ESP32 LEDC Channels)
#define SERVO_PWM_FREQ_HZ             (50)  // Standard 50Hz RC servo refresh rate
#define SERVO_PWM_RESOLUTION_BITS     (16)  // 16-bit resolution for smooth position control
#define SERVO_PWM_CHANNEL_WAIST       (4)   // LEDC Channel 4
#define SERVO_PWM_CHANNEL_SHOULDER    (5)   // LEDC Channel 5
#define SERVO_PWM_CHANNEL_ELBOW       (6)   // LEDC Channel 6
#define SERVO_PWM_CHANNEL_WRIST       (7)   // LEDC Channel 7
#define SERVO_PWM_CHANNEL_GRIPPER     (8)   // LEDC Channel 8

/* ----------------------------------------------------------------------------
 * 4. Servo Safe Calibration Limits & Park Positions (TBD)
 * ----------------------------------------------------------------------------
 * All angle boundaries (degrees) and pulse width limits (microseconds)
 * must be verified on the physical mechanical assembly to prevent binding.
 */
// Pulse width defaults (500us to 2500us standard for 180/270 deg digital servos)
#define SERVO_PULSE_MIN_US            (500)
#define SERVO_PULSE_MAX_US            (2500)

// Joint Angle Travel Limits (Degrees)
#define WAIST_MIN_DEG                 (0)
#define WAIST_MAX_DEG                 (180) // TBD (pending 180 vs 270 deg horn configuration)
#define WAIST_HOME_DEG                (90)  // TBD: Centered forward

#define SHOULDER_MIN_DEG              (0)   // TBD: Lower physical stop
#define SHOULDER_MAX_DEG              (180) // TBD: Upper physical stop
#define SHOULDER_HOME_DEG             (90)  // TBD: Neutral resting angle

#define ELBOW_MIN_DEG                 (0)   // TBD
#define ELBOW_MAX_DEG                 (180) // TBD
#define ELBOW_HOME_DEG                (90)  // TBD

#define WRIST_MIN_DEG                 (0)   // TBD
#define WRIST_MAX_DEG                 (180) // TBD
#define WRIST_HOME_DEG                (90)  // TBD

#define GRIPPER_OPEN_DEG              (0)   // TBD: Full open position
#define GRIPPER_CLOSED_DEG            (90)  // TBD: Full closed position
#define GRIPPER_HOME_DEG              (0)   // TBD: Open resting state

/* ----------------------------------------------------------------------------
 * 5. RoboLink Mobile Application Protocol & Control Keys
 * ----------------------------------------------------------------------------
 * Communication: UDP broadcast over Wi-Fi
 * Default IP:    192.168.4.1 (ESP32 AP Mode)
 * Default Port:  4210
 */
#define ROBOLINK_WIFI_SSID            "RoboReach_AP"
#define ROBOLINK_WIFI_PASS            "roboreach123" // Minimum 8 characters
#define ROBOLINK_UDP_PORT             (4210)
#define ROBOLINK_TIMEOUT_MS           (500)          // Failsafe triggers if no packets for 500ms

// Documented Logical Keys from Mobile App
#define KEY_BASE_THROTTLE             "throttle"     // Base forward (+255) / backward (-255)
#define KEY_BASE_STEER                "steer"        // Base left (-255) / right (+255)
#define KEY_ARM_WAIST                 "waist"        // Waist rotation (0 to 180)
#define KEY_ARM_SHOULDER              "shoulder"     // Shoulder elevation (0 to 180)
#define KEY_ARM_ELBOW                 "elbow"        // Elbow extension (0 to 180)
#define KEY_ARM_WRIST                 "wrist"        // Wrist pitch (0 to 180)
#define KEY_ARM_GRIPPER               "gripper"      // Gripper open/close (0 or 1, or 0 to 90)

/* ----------------------------------------------------------------------------
 * 6. Electrical Power Architecture Notes (TBD)
 * ----------------------------------------------------------------------------
 * - Battery: TBD (Chemistry, voltage, capacity, C-rating)
 * - Buck Converter: TBD (Exact module, voltage, current capacity)
 * - Dedicated Servo Rail: TBD (Annimos 60kg high-surge current isolation)
 */
#define POWER_BATTERY_CELLS_TBD       1
#define POWER_SERVO_RAIL_VOLTAGE_TBD  1

#endif // ROBOREACH_CONFIG_H
