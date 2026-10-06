# RoboReach Software Architecture Specification

**Document Version**: 1.0  
**Hardware Authority**: `ROBOREACH BOM - Sheet1.pdf`  
**Target Platform**: ESP32 Microcontroller  
**Status**: Architecture Skeleton Complete / Hardware GPIOs Inactive (TBD)

---

## 1. System Overview & Communication Flow

RoboReach is an integrated mobile robotic platform featuring a differential drive base and a 5-DOF articulated robotic arm. The robot communicates wirelessly with the **RoboLink** mobile application over Wi-Fi using the UDP transport protocol.

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
                      |   ESP32 (SoftAP Mode)       |
                      |   SSID: RoboReach_AP        |
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
                   +-----------------+-----------------+
                   |                                   |
                   v                                   v
    +-----------------------------+     +-----------------------------+
    |       motor_control         |     |       servo_control         |
    |  - Deadband filtering       |     |  - Safe angle clamping      |
    |  - Differential drive mix   |     |  - 5-DOF pose coordination  |
    |  - Target duty calculation  |     |  - Park / Home sequence     |
    +-----------------------------+     +-----------------------------+
                   |                                   |
        [Gated: Awaiting Pins]               [Gated: Awaiting Pins]
                   |                                   |
                   v                                   v
    +-----------------------------+     +-----------------------------+
    |  BTS7960 Driver (43A)       |     |  Servo PWM Power Rails      |
    |  2 x 12V 100RPM Motors      |     |  • Waist (DS3225 25kg)      |
    |  100mm Drive Wheels         |     |  • Shoulder (Annimos 60kg)  |
    |                             |     |  • Elbow (DS3225 25kg)      |
    |                             |     |  • Wrist (DS3225 25kg)      |
    |                             |     |  • Gripper (MG90 Metal)     |
    +-----------------------------+     +-----------------------------+
```

---

## 2. Software Modules Breakdown

The codebase is organized into modular units located in [`src/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src) and [`include/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/include):

### 2.1 Configuration Header: [`include/config.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/include/config.h)
- Serves as the single source of truth for software parameters and hardware pinouts.
- Contains placeholders for:
  - ESP32 board variant definition
  - BTS7960 motor driver GPIO pins (Left/Right forward, reverse, and enable lines)
  - 5 servo signal GPIO pins
  - LEDC timer frequencies and resolution channels
  - Mechanical angle boundaries and safe home/park positions
  - RoboLink network credentials (SSID, password, port) and control key names.
- **Safety Guarantee**: All unknown pins are assigned to `-1`.

### 2.2 Network & Communication: [`src/robolink_handler.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/robolink_handler.h) & [`src/robolink_handler.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/robolink_handler.cpp)
- Encapsulates the official **RoboLink** library (`RoboLink.h`).
- Sets up the ESP32 Wi-Fi SoftAP (`RoboReach_AP`, IP `192.168.4.1`, UDP port `4210`).
- Drains UDP packets non-blockingly during every loop cycle.
- Automatically handles timeout detection: if no packets arrive within `ROBOLINK_TIMEOUT_MS` (500 ms), the handler marks the connection as lost and zeros out throttle and steer.
- Exposes clean accessors to retrieve parsed integer control values.

### 2.3 Mobile Base Subsystem: [`src/motor_control.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/motor_control.h) & [`src/motor_control.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/motor_control.cpp)
- Implements 2-wheel differential drive kinematics:
  $$\text{LeftDuty} = \text{constrain}(\text{Throttle} + \text{Steer}, -255, 255)$$
  $$\text{RightDuty} = \text{constrain}(\text{Throttle} - \text{Steer}, -255, 255)$$
- Implements deadband filtering (`MOTOR_INPUT_DEADBAND`) to eliminate joystick center jitter.
- Stores logical motor state (`leftTargetDuty`, `rightTargetDuty`, `isStopped`).
- **Hardware Gating**: Checks if any motor pin is `-1`. If so, physical GPIO writes and timer configurations are bypassed, preventing accidental motor activation while hardware wiring is unconfirmed.

### 2.4 Articulated Arm Subsystem: [`src/servo_control.h`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/servo_control.h) & [`src/servo_control.cpp`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/src/servo_control.cpp)
- Manages target angular positions (0° to 180°) for the 5 arm actuators:
  1. **Waist**: Base rotation (DS3225 25kg)
  2. **Shoulder**: Primary lift joint (Annimos 60kg)
  3. **Elbow**: Forearm elevation (DS3225 25kg)
  4. **Wrist**: Wrist pitch (DS3225 25kg)
  5. **Gripper**: End effector clamp (MG90 Metal)
- Enforces strict software boundary clamping against min/max angle limits defined in `config.h`.
- Provides `servoControlPark()` to restore joints to default neutral resting angles.
- **Hardware Gating**: Checks if any servo pin is `-1`. If so, servo attach and PWM generation are bypassed.

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
| **Arm Joint 1** | `"waist"` | 0° to 180° | Base yaw rotation (90° = centered) |
| **Arm Joint 2** | `"shoulder"` | 0° to 180° | Main arm lift / elevation (90° = neutral) |
| **Arm Joint 3** | `"elbow"` | 0° to 180° | Forearm reach / extension (90° = neutral) |
| **Arm Joint 4** | `"wrist"` | 0° to 180° | Wrist tilt angle (90° = level) |
| **End Effector**| `"gripper"` | 0° to 90° | Gripper open (0°) to fully clamped (90°) |

---

## 4. Current State: What is Ready vs. What Depends on Electrical Team

### What is Completed & Ready (Software Layer)
1. **Full Software Skeleton**: Modular, clean C++ code in `src/` and `include/` compiling with ESP32 Arduino framework.
2. **Verified Wireless Protocol**: Official RoboLink UDP communication logic, packet parsing, and timeout watchdog implemented.
3. **Differential Drive Mathematics**: Kinematic mixing (`throttle ± steer`), deadband filtering, and target duty generation.
4. **Arm Boundary Protection**: Angle limiting, joint coordinate tracking, and park/home pose management.
5. **Safety Isolation**: Hardcoded `-1` pin gating ensuring zero hardware outputs are driven until pins are assigned.
6. **Communication Verification Suite**: Standalone and library-based test sketches in [`tests/robolink_comm_test/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/tests/robolink_comm_test).

### What Depends on the Electrical Team
1. **ESP32 Variant & Exact Form Factor**:
   - Total exposed GPIO pins, internal pin numbering, and 3.3V vs. 5V logic compatibility.
2. **BTS7960 Driver Pin Connections**:
   - Specific GPIOs connected to `RPWM`, `LPWM`, and `R_EN`/`L_EN` (bridged or separate).
   - Confirmation whether single or dual BTS7960 driver modules are wired for the 2 DC motors.
3. **Servo Signal GPIO Allocation**:
   - 5 dedicated PWM-capable ESP32 output pins (avoiding strapping pins: 0, 2, 12, 15 and input-only pins: 34, 35, 36, 39).
4. **Power Rail Architecture & Buck Converter Topology**:
   - Confirmation of regulated voltages (e.g., 6.8V / 7.4V rail for Annimos 60kg and DS3225 servos; 5V rail for logic).
   - Peak current delivery capacity to prevent brownout during simultaneous arm movements.
5. **Physical Mechanical Calibration**:
   - Verification of servo horns, GT2 belt tensioning, and physical stop limits (min/max angles in degrees and corresponding microsecond pulse widths).
