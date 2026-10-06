# RoboReach Final Project Specification

**Authoritative Source**: `ROBOREACH BOM - Sheet1.pdf`  
**Status**: ACTIVE / AUTHORITATIVE (Supersedes all prototype and testing specifications)  
**Last Updated**: October 2026

---

## 1. Project Overview & System Architecture

RoboReach is an ESP32-controlled mobile robotic platform equipped with a high-torque 5-axis/articulated robotic arm and end effector, controlled remotely via the RoboLink mobile application over Wi-Fi.

### High-Level Architecture

```
                       +-----------------------------+
                       |    RoboLink Mobile App      |
                       +-----------------------------+
                                      |
                                      v [Wi-Fi / Network Protocol TBD]
                       +-----------------------------+
                       |        ESP32 (MCU)          |
                       +-----------------------------+
                                      |
            +-------------------------+-------------------------+
            |                                                   |
            v                                                   v
+-----------------------+                           +-----------------------+
|  BTS7960 Motor Driver |                           |  Servo Control Rails  |
+-----------------------+                           +-----------------------+
            |                                                   |
            v                                                   v
+-----------------------+                       +-------------------------------+
| 2 × 12V 100RPM Motors |                       | Arm Actuators:                |
+-----------------------+                       | • Waist: DS3225 25kg          |
            |                                   | • Shoulder: Annimos 60kg      |
            v                                   | • Elbow: DS3225 25kg          |
+-----------------------+                       | • Wrist: DS3225 25kg          |
| Mobile Base (100mm)   |                       | • End Effector: MG90 Metal    |
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
| 4 | **Microcontroller** | ESP32 | 1 | Dual-core Wi-Fi/BLE MCU (Exact board form factor TBD) | Confirmed |
| 5 | **Buck Converter** | DC-DC Step-Down Buck Converter | 1 | Voltage regulator (Exact model & rating TBD) | Confirmed in BOM |
| 6 | **Wheels** | 100mm Diameter Wheels | 2+ | Drive wheels for differential mobile base | Confirmed |
| 7 | **Connectors** | System Interconnects & Terminals | As req. | Headers, screw terminals, power connectors | Confirmed in BOM |
| 8 | **Wiring (Red)** | 20 AWG Ultra-Flexible Silicone Wire | 1 roll | Motor & high-current power routing | Confirmed |
| 9 | **Wiring (Black)**| 20 AWG Ultra-Flexible Silicone Wire | 1 roll | Ground & high-current return routing | Confirmed |
| 10 | **Wiring (Colored)**| Multi-color Signal / Logic Wire | As req. | Logic, PWM signal, and sensor hookups | Confirmed in BOM |
| 11 | **Waist Joint** | DS3225 25kg Digital Servo | 1 | Base rotational joint (high torque, metal gear) | Confirmed |
| 12 | **Shoulder Joint** | Annimos 60kg Digital Servo | 1 | Primary shoulder lift joint (ultra-high torque) | Confirmed |
| 13 | **Elbow Joint** | DS3225 25kg Digital Servo | 1 | Forearm elevation joint | Confirmed |
| 14 | **Wrist Joint** | DS3225 25kg Digital Servo | 1 | Wrist pitch/rotation joint | Confirmed |
| 15 | **End Effector** | MG90 Metal Gear Micro Servo | 1 | Gripper / clamp mechanism | Confirmed |
| 16 | **Timing Belts** | GT2 Open Timing Belt (6mm W, 2mm P, 1m L) | 3 | Arm joint / mechanical reduction transmissions | Confirmed |
| 17 | **Control Software**| RoboLink Mobile Application | 1 | Wi-Fi remote interface & telemetry | Confirmed |

---

## 3. Subsystem Breakdown

### 3.1 Mobile Base Subsystem
- **Drive Configuration**: 2-wheel differential drive with 100mm wheels and idler/caster support.
- **Actuation**: 2 × 12V 100 RPM DC motors driven by a BTS7960 H-bridge.
- **Speed & Torque Profile**: 100 RPM gives higher linear travel speed compared to 60 RPM prototype motors while maintaining sufficient stall torque through the BTS7960 driver.

### 3.2 5-DOF Articulated Arm & Gripper Subsystem
The robotic arm consists of 5 dedicated servos:
1. **Waist (Base Yaw)**: DS3225 (25 kg·cm stall torque at 6.8V, metal gear, waterproof).
2. **Shoulder (Pitch 1)**: Annimos 60kg (60–70 kg·cm torque rating, steel/aluminum gear, high-voltage capable).
3. **Elbow (Pitch 2)**: DS3225 (25 kg·cm torque).
4. **Wrist (Pitch 3 / Roll)**: DS3225 (25 kg·cm torque).
5. **End Effector (Gripper)**: MG90 Metal Gear micro servo (1.8–2.2 kg·cm torque).
- **Belt Drives**: 3 × GT2 open timing belts (6mm width, 2mm pitch) for transmission reduction, backlash mitigation, and joint decoupling.

### 3.3 Power Distribution Network
- **Motor Power (12V Rail)**: Supplies 12V directly to the BTS7960 motor driver power terminals (`VCC` / `GND`).
- **Logic & MCU Power**: Stepped down via buck converter from main battery pack to 5V / 3.3V.
- **Servo Power**: Requires dedicated regulated rail(s) capable of delivering high peak surge currents (Annimos 60kg alone can draw >3–4A under stall; 3 × DS3225 can draw 2.5A each under load).

---

## 4. Unconfirmed Specifications & Open Parameters (Strictly TBD)

The following parameters must **NOT** be assumed or hardcoded until explicitly confirmed:

| Parameter Category | Item | Status | Impact / Requirement |
| :--- | :--- | :--- | :--- |
| **MCU Board** | Exact ESP32 module / DevKit variant | **TBD** | Determines available pin count, internal ADC pins, strapping pins |
| **GPIO Allocation** | BTS7960 Control Pins (`RPWM`, `LPWM`, `R_EN`, `L_EN`, `R_IS`, `L_IS`) | **TBD** | Requires 2 PWM-capable pins + enable lines |
| **GPIO Allocation** | 5 Servo PWM signal pins | **TBD** | Requires 5 dedicated ESP32 LEDC hardware PWM channels |
| **Power Supply** | Battery pack chemistry, cell count, capacity, & discharge rating | **TBD** | e.g., 3S LiPo, 3S/4S LiFePO4, 18650 pack (voltage & safe cutoff) |
| **Buck Converter** | Exact converter model and output rating | **TBD** | Must confirm output voltage (5V vs 6V vs 7.4V) and continuous amperage |
| **Servo Power Topology** | Single vs dual buck converter rails | **TBD** | Annimos 60kg operates at 6.0V–8.4V; DS3225 operates at 4.8V–6.8V; MG90 operates at 4.8V–6.0V |
| **BTS7960 Mode** | Drive interface mode | **TBD** | Dual-PWM direction control vs PWM + Direction toggling |
| **RoboLink Protocol** | Network transport protocol & packet schema | **TBD** | WebSocket, UDP datagrams, TCP socket, or HTTP REST API |
| **RoboLink Control Map** | Channel / control mapping | **TBD** | Joystick/slider mappings to wheel differentials and joint angles |
| **Joint Kinematics** | Physical link lengths & travel limits | **TBD** | Min/max angles, safe home positions, pulse width limits (µs) |

---

## 5. Development Roadmap & Constraints

1. **Strict No-Assumptions Rule**: Firmware code must not hardcode GPIOs, power assumptions, or communication payloads based on prototype drafts.
2. **Modular Architecture**:
   - `include/config.h`: Hardware pinouts, constants, and calibration (to be populated once TBDs are finalized).
   - `lib/drive/`: BTS7960 DC motor driver control using ESP32 LEDC PWM.
   - `lib/arm/`: 5-servo coordinate / pulse width management using ESP32 LEDC.
   - `lib/comms/`: RoboLink Wi-Fi communication handler.
   - `lib/safety/`: Brownout prevention, watchdog timer, battery voltage monitoring, and emergency stop.
3. **Non-Blocking Operation**: Firmware will adhere to strict non-blocking design (`millis()` timers, asynchronous network callbacks, FreeRTOS tasks where appropriate).
