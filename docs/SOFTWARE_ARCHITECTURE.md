# RoboReach Software Architecture Specification

**Document Version**: 1.0  
**Hardware Authority**: `ROBOREACH BOM - Sheet1.pdf`  
**Target Platform**: ESP32 Microcontroller  
**Status**: Architecture Complete / Hardware Confirmed

---

## 1. System Overview & Communication Flow

RoboReach is an integrated mobile robotic platform featuring a differential drive base and a 2-DOF articulated robotic arm. The robot communicates wirelessly with the **RoboLink** mobile application over Wi-Fi using the UDP transport protocol.

### End-to-End Data Flow

```
                      +-----------------------------+
                      |   RoboLink Mobile App       |
                      |   (Phone UI: Sticks/Sliders)|
                      +-----------------------------+
                     |
                     | Wi-Fi (UDP port 4210)
                     v
                      +-----------------------------+
                      |        ESP32 (MCU)          |
                      +-----------------------------+
                     |
                    v
             +-----------------------------+
             |     robolink_handler        |
             |  - Non-blocking UDP parse   |
             |  - Key-value extraction     |
             |  - Failsafe timeout monitor |
             +-----------------------------+
                |
                v
       +-----------------+-----------------+
       |                                   |
       v                                   v
   +-----------------------------+     +-----------------------------+
   |       motor_control         |     |       servo_control         |
   |  - Deadband filtering       |     |  - Safe angle clamping      |
   |  - Differential drive mix   |     |  - 2-DOF pose coordination |
   |  - Target duty calculation  |     |  - Park / Home sequence   |
   +-----------------------------+     +-----------------------------+
```

---

## 2. Software Modules Breakdown

The codebase is organized into modular units located in [`src/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src) and [`include/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/include):

### 2.1 Configuration Header: [`include/config.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/include/config.h)
- Serves as the single source of truth for software parameters and hardware pinouts.
- Contains confirmed pin assignments (no TBD placeholders for GPIOs):
  - BTS7960 motor driver GPIO pins (Left/Right forward, reverse, and enable lines)
  - 2-DOF servo signal GPIO pins: SHOULDER=18, ELBOW=19, GRIPPER=21
  - LEDC timer frequencies and resolution channels
  - Mechanical angle boundaries and safe home/park positions
  - RoboLink network credentials (SSID, password, port) and control key names.
- **Safety Guarantee**: All pins are assigned (no -1 placeholders).

### 2.2 Network & Communication: [`src/robolink_handler.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/robolink_handler.h) & [`src/robolink_handler.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/robolink_handler.cpp)
- Encapsulates the official **RoboLink** library (`RoboLink.h`).
- Sets up the ESP32 Wi-Fi SoftAP (`RoboReach`, IP `192.168.4.1`, UDP port `4210`).
- Drains UDP packets non-blockingly during every loop cycle.
- Automatically handles timeout detection: if no packets arrive within `ROBOLINK_TIMEOUT_MS` (500 ms), the handler marks the connection as lost and zeros out throttle and steer.
- Exposes clean accessors to retrieve parsed integer control values.

### 2.3 Mobile Base Subsystem: [`src/motor_control.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/motor_control.h) & [`src/motor_control.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/motor_control.cpp)
- Implements 2-wheel differential drive kinematics:
  $$\text{LeftDuty} = \text{constrain}(\text{Throttle} + \text{Steer}, -255, 255)$$
  $$\text{RightDuty} = \text{constrain}(\text{Throttle} - \text{Steer}, -255, 255)$$
- Implements deadband filtering (`MOTOR_INPUT_DEADBAND`) to eliminate joystick center jitter.
- Stores logical motor state (`leftTargetDuty`, `rightTargetDuty`, `isStopped`).

### 2.4 2-DOF Articulated Arm Subsystem: [`src/servo_control.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/servo_control.h) & [`src/servo_control.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/servo_control.cpp)
- Manages target angular positions for the 3 arm actuators:
  1. **Shoulder**: Primary lift joint (Annimos 60kg)
  2. **Elbow**: Forearm elevation joint (20kg)
  3. **Gripper**: End effector clamp (SG90 Metal)
- Enforces strict software boundary clamping against min/max angle limits defined in `config.h`.
- Provides `servoControlPark()` to restore joints to default neutral resting angles.

### 2.5 Main Application Orchestrator: [`src/main.ino`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/main.ino)
- Coordinates all subsystems in a strictly non-blocking architecture.
- Dispatches parsed RoboLink inputs to motor and servo handlers every loop iteration.
- Enforces an automated safety failsafe: if connection times out, base motors are commanded to immediate stop.
- Logs a 2-second diagnostic status summary over the Serial Monitor (115200 baud).

---

## 3. Planned Logical Control Mapping

The following mapping links the RoboLink mobile application widgets directly to robot motion:

| Subsystem | RoboLink Key | Input Range | Planned Robot Behavior |
| :--- | :--- | :--- | :--- |
| **Base** | `"throttle"` | -255 to +255 | Forward (+) and Reverse (-) translation |
| **Base** | `"steer"` | -255 to +255 | Left (-) and Right (+) differential turning |
| **Arm Joint 1** | `"arm_x"` | 0° to 180° | Shoulder lift (90° = centered) |
| **Arm Joint 2** | `"arm_y"` | 0° to 180° | Elbow extension (90° = neutral) |
| **End Effector**| `"gripper"` | 0° to 90° | Gripper open (0°) to fully clamped (90°) |

---

## 4. Current State: What is Ready vs. What Depends on Electrical Team

### What is Completed & Ready (Software Layer)
1. **Full Software Skeleton**: Modular, clean C++ code in `src/` and `include/` compiling with ESP32 Arduino framework.
2. **Verified Wireless Protocol**: Official RoboLink UDP communication logic, packet parsing, and timeout watchdog implemented.
3. **Differential Drive Mathematics**: Kinematic mixing (`throttle ± steer`), deadband filtering, and target duty generation.
4. **Arm Boundary Protection**: Angle limiting, joint coordinate tracking, and park/home pose management.
5. **Safety Isolation**: Hardcoded pin assignments ensuring hardware outputs are properly initialized.

### What Depends on Electrical Team / Hardware Calibration
1. **ESP32 Variant & Exact Form Factor**: Confirmed: esp32dev.
2. **BTS7960 Driver Pin Connections**: Confirmed: LEFT_RPWM=25, LEFT_LPWM=26, RIGHT_RPWM=27, RIGHT_LPWM=33.
3. **Servo Signal GPIO Allocation**: Confirmed: SHOULDER=18, ELBOW=19, GRIPPER=21.
4. **Power Rail Architecture & Buck Converter Topology**: To be confirmed and calibrated on hardware.
5. **Arm Link Lengths**: L1=150 mm, L2=120 mm (per task specification).
6. **Physical Mechanical Calibration**: Servo horn configurations and physical stop limits should be verified on the actual hardware.

---

## 5. Development Roadmap & Constraints

1. **Strict No-Assumptions Rule**: Firmware code uses confirmed pin assignments from `config.h`.
2. **Modular Architecture**:
   - `include/config.h`: Hardware pinouts, constants, and calibration (confirmed values).
   - `lib/drive/`: BTS7960 DC motor driver control using ESP32 LEDC PWM.
   - `lib/arm/`: 2-DOF coordinate / pulse width management using ESP32 LEDC.
   - `lib/comms/`: RoboLink Wi-Fi communication handler.
   - `lib/safety/`: Brownout prevention, watchdog timer, battery voltage monitoring, and emergency stop.
3. **Non-Blocking Operation**: Firmware will adhere to strict non-blocking design (`millis()` timers, asynchronous network callbacks, FreeRTOS tasks where appropriate).
4. **Hardware Calibration Required**:
   - Servo pulse width calibration (verify SG90 and digital servos on actual arm)
   - Arm link length verification (L1=150mm, L2=120mm)
   - Physical stop limit confirmation
   - Gripper jaw travel measurement