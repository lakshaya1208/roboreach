# RoboReach Final Project Specification

**Authoritative Source**: `ROBOREACH BOM - Sheet1.pdf`  
**Status**: ACTIVE / AUTHORITATIVE (Supersedes all prototype and testing specifications)  
**Last Updated**: October 2026

---

## 1. Project Overview & System Architecture

RoboReach is an ESP32-controlled mobile robotic platform featuring a differential drive base and a 2-DOF articulated robotic arm. The robot communicates remotely via the RoboLink mobile application over Wi-Fi.

### High-Level Architecture

```
                        +-----------------------------+
                        |    RoboLink Mobile App      |
                        +-----------------------------+
                                       |
                                       v [Wi-Fi / Network Protocol]
                        +-----------------------------+
                        |        ESP32 (MCU)          |
                        +-----------------------------+
                                       |
             +-------------------------+-------------------------+
             |                                                   |
             v                                                   v
+-----------------------+                           +-----------------------+
|  BTS7960 Motor Driver |                           |  2-DOF Servo Control  |
+-----------------------+                           +-----------------------+
             |                                                   |
             v                                                   v
+-----------------------+                       +-------------------------------+
| 2 × 12V 100RPM Motors |                       | Arm Actuators:                |
+-----------------------+                       | • Shoulder: Annimos 60kg    |
            |                                   | • Elbow: 20kg                 |
            v                                   | • Gripper: SG90 Metal         |
+-----------------------+                       +-------------------------------+
| Mobile Base (100mm)   |                       | GT2 Direct Drive (no belts)   |
+-----------------------+                       +-------------------------------+
```

---

## 2. Final Hardware Specification (Authoritative Final BOM)

The following components are confirmed directly from the authoritative project BOM (`ROBOREACH BOM - Sheet1.pdf`):

| S.No | Subsystem / Part | Component / Specification | Quantity | Notes / Source Details | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | **Chassis** | 3D Printed PETG / ABS | 1 | Structural chassis for base & arm mounting | Confirmed |
| 2 | **Drive Motors** | 12V 100 RPM Geared DC Motors | 2 | Grade-A Johnson geared DC motors | Confirmed |
| 3 | **Motor Driver** | BTS7960 High-Power Motor Driver | 1 | High-current H-bridge module (up to 43A) | Confirmed |
| 4 | **Microcontroller** | ESP32 | 1 | Dual-core Wi-Fi/BLE MCU (esp32dev board) | Confirmed |
| 5 | **Buck Converter** | DC-DC Step-Down Buck Converter | 1 | Voltage regulator (Confirmed in BOM) | Confirmed |
| 6 | **Wheels** | 100mm Diameter Wheels | 2+ | Drive wheels for differential mobile base | Confirmed |
| 7 | **Connectors** | System Interconnects & Terminals | As req. | Headers, screw terminals, power connectors | Confirmed in BOM |
| 8 | **Wiring (Red)** | 20 AWG Ultra-Flexible Silicone Wire | 1 roll | Motor & high-current power routing | Confirmed |
| 9 | **Wiring (Black)**| 20 AWG Ultra-Flexible Silicone Wire | 1 roll | Ground & high-current return routing | Confirmed |
| 10 | **Wiring (Colored)**| Multi-color Signal / Logic Wire | As req. | Logic, PWM signal, and sensor hookups | Confirmed in BOM |
| 11 | **Shoulder Joint** | Annimos 60kg Digital Servo | 1 | Base rotational joint (high torque, metal gear) | Confirmed |
| 12 | **Elbow Joint** | 20kg Digital Servo | 1 | Forearm elevation joint | Confirmed |
| 13 | **End Effector** | SG90 Metal Gear Micro Servo | 1 | Gripper / clamp mechanism | Confirmed |
| 14 | **Timing Belts** | N/A (Direct drive) | - | Removed - 2-DOF arm uses direct servo attachments | Confirmed (deleted) |

---

## 3. Subsystem Breakdown

### 3.1 Mobile Base Subsystem
- **Drive Configuration**: 2-wheel differential drive with 100mm wheels and idler/caster support.
- **Actuation**: 2 × 12V 100 RPM DC motors driven by a BTS7960 H-bridge.
- **Speed & Torque Profile**: 100 RPM gives higher linear travel speed compared to 60 RPM prototype motors while maintaining sufficient stall torque through the BTS7960 driver.

### 3.2 2-DOF Articulated Arm & Gripper Subsystem
The robotic arm consists of 2 dedicated servos:
1. **Shoulder (Pitch 1)**: Annimos 60kg (60–70 kg·cm torque rating, steel/aluminum gear, high-voltage capable).
2. **Elbow (Pitch 2)**: 20kg Digital Servo (forearm elevation joint).
3. **End Effector (Gripper)**: SG90 Metal Gear micro servo (gripper / clamp mechanism).
- **Transmission**: Direct drive to servo horns (GT2 timing belts removed - simplified 2-DOF mechanical design).

### 3.3 Power Distribution Network
- **Motor Power (12V Rail)**: Supplies 12V directly to the BTS7960 motor driver power terminals (`VCC` / `GND`).
- **Logic & MCU Power**: Stepped down via buck converter from main battery pack to 5V / 3.3V.
- **Servo Power**: Requires dedicated regulated rail(s) capable of delivering high peak surge currents (Annimos 60kg alone can draw >3–4A under stall; 20kg servo can draw ~1A under load).

---

## 4. Unconfirmed Specifications & Open Parameters (Strictly TBD)

The following parameters must **NOT** be assumed or hardcoded until explicitly confirmed:

| Parameter Category | Item | Status | Impact / Requirement |
| :--- | :--- | :--- | :--- |
| **MCU Board** | Exact ESP32 module / DevKit variant | **Confirmed**: esp32dev | Determines available pin count, internal ADC pins, strapping pins |
| **GPIO Allocation** | BTS7960 Control Pins (`RPWM`, `LPWM`, `R_EN`, `L_EN`) | **Confirmed**: LEFT_RPWM=25, LEFT_LPWM=26, RIGHT_RPWM=27, RIGHT_LPWM=33 | Requires 2 PWM-capable pins + enable lines per channel |
| **GPIO Allocation** | 3 Servo PWM signal pins | **Confirmed**: SHOULDER=18, ELBOW=19, GRIPPER=21 | 3 dedicated ESP32 LEDC hardware PWM channels |
| **Power Supply** | Battery pack chemistry, cell count, capacity, & discharge rating | **TBD** | e.g., 3S LiPo, 3S/4S LiFePO4, 18650 pack (voltage & safe cutoff) |
| **Arm Link Lengths** | L1 (shoulder-to-elbow), L2 (elbow-to-gripper) | **Confirmed**: L1=150mm, L2=120mm | Per task specification - must verify on hardware |
| **Servo Power Topology** | Single vs dual buck converter rails | **TBD** | Annimos 60kg operates at 6.0V–8.4V; 20kg and SG90 operate at 4.8V–6.0V |
| **BTS7960 Mode** | Drive interface mode | **Confirmed**: Dual-PWM direction control | Verified per pin assignments in config.h |
| **RoboLink Protocol** | Network transport protocol & packet schema | **Confirmed**: UDP over Wi-Fi | Port 4210, SoftAP mode SSID: RoboReach |
| **RoboLink Control Map** | Channel / control mapping | **Confirmed**: throttle, steer, arm_x, arm_y, grip | Mapped to specific widgets in RoboLink mobile app |

---

## 5. Development Roadmap & Constraints

1. **Strict No-Assumptions Rule**: Firmware code must use confirmed pin assignments from `include/config.h`. No random pin assignments.

2. **Modular Architecture**:
   - `include/config.h`: Hardware pinouts, constants, and calibration (confirmed values).
   - `lib/drive/`: BTS7960 DC motor driver control using ESP32 LEDC PWM.
   - `lib/arm/`: 2-DOF coordinate / pulse width management using ESP32 LEDC.
   - `lib/comms/`: RoboLink Wi-Fi communication handler.
   - `lib/safety/`: Brownout prevention, watchdog timer, battery voltage monitoring, and emergency stop.

3. **Non-Blocking Operation**: Firmware will adhere to strict non-blocking design (`millis()` timers, asynchronous network callbacks, FreeRTOS tasks where appropriate).

4. **Hardware Calibration Required**:
   - Servo pulse width verification on actual arm (SG90, Annimos 60kg, 20kg)
   - Arm link length verification (L1=150mm, L2=120mm) - confirm on physical assembly
   - Physical stop limit confirmation for shoulder and elbow joints
   - Gripper jaw travel measurement and open/close angle calibration

5. **Important**: All `-1` TBD placeholders have been removed from `include/config.h`. All pin assignments are confirmed per the task specification.