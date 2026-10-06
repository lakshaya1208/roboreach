---
name: roboreach-robot
description: Project-specific instructions, authoritative hardware specifications, and development guidelines for the RoboReach ESP32 robot.
---
# RoboReach Robot Skill

This skill contains project-specific instructions, authoritative hardware specifications, and development guidelines for the RoboReach robot.

## Authoritative Hardware Specifications
The authoritative source for the final RoboReach hardware is `ROBOREACH BOM - Sheet1.pdf` and [`docs/FINAL_PROJECT_SPEC.md`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/docs/FINAL_PROJECT_SPEC.md). All older prototype specifications are documented for reference only in [`docs/PROTOTYPE_REFERENCE.md`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/docs/PROTOTYPE_REFERENCE.md) and MUST NOT override the final BOM.

- **Chassis**: 3D printed PETG / ABS
- **Microcontroller**: ESP32 (exact board variant `TBD`)
- **Control Software**: RoboLink mobile app via Wi-Fi
- **Drive Motors**: 2 × 12V 100RPM DC motors
- **Motor Driver**: BTS7960 high-power motor driver (43A H-bridge)
- **Wheels**: 100mm diameter
- **Robotic Arm & Servos (5 Actuators)**:
  - Waist Joint: DS3225 25kg digital servo
  - Shoulder Joint: Annimos 60kg digital servo
  - Elbow Joint: DS3225 25kg digital servo
  - Wrist Joint: DS3225 25kg digital servo
  - End Effector: MG90 metal gear micro servo
- **Timing Belts**: 3 × GT2 open timing belt (6mm width, 2mm pitch, 1m length)
- **Power & Wiring**:
  - Buck converter (exact model `TBD`)
  - Red & Black 20AWG silicone wire for motors
  - Signal wiring and connectors as specified in BOM

## Unconfirmed / Pending Hardware Details (Strictly Marked as TBD)
Do NOT assume or invent the following specifications until explicitly confirmed:
- **ESP32 Variant & Pinout**: `TBD` (do not assume DevKit V1 or pin mappings)
- **ESP32 GPIO Pin Assignments**: `TBD` (motors, enables, servos)
- **Battery Specification**: `TBD` (voltage, chemistry, cell configuration, capacity)
- **Buck Converter Model & Output Ratings**: `TBD` (exact output voltage & continuous current)
- **BTS7960 Wiring & Mode**: `TBD` (dual-PWM vs PWM+DIR, logic level)
- **Servo Power Rail Architecture**: `TBD` (regulator topology and peak current capacity)
- **Servo GPIO Assignments**: `TBD`
- **Servo Angle Limits**: `TBD` (safe min/max angles and pulse widths)
- **RoboLink Communication Protocol**: `TBD` (transport protocol, port, packet format, endpoint)
- **RoboLink-to-Joint Mapping**: `TBD` (channel assignment, UI controls to arm/base movements)
- **Arm Kinematics & Link Geometry**: `TBD`

## Development & Firmware Guidelines
- Follow both `Arduino_Programmer` and `roboreach-robot` skills for all development.
- Never invent or assume GPIO pins, driver configurations, power systems, or protocol formats.
- Keep motor control, arm control, communication, and safety logic modular across `src/`, `include/`, `lib/`, and `tests/`.
- Ensure all execution loops are non-blocking (preferring `millis()` over `delay()`).
- Always request confirmation for `TBD` parameters before generating code that depends on them.

