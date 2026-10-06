# RoboReach
ESP32-based mobile robot with reach capability.

## Hardware Specification (Authoritative)
- **Platform**: ESP32 Dev Module (ESP32-WROOM-32)
- **Arm**: 2-DOF Only - Shoulder (Annimos 60kg servo) + Elbow (20kg servo) + SG90 Gripper
- **Drive**: 2x IBT-2 with 2x 12V 60RPM Johnson motors, skid steering
- **Pins**: LEFT_RPWM=25, LEFT_LPWM=26, RIGHT_RPWM=27, RIGHT_LPWM=33, SHOULDER=18, ELBOW=19, GRIPPER=21
- **Arm geometry**: L1=150 mm (shoulder-to-elbow), L2=120 mm (elbow-to-gripper)
- **Wi-Fi**: RoboReach AP, UDP port 4210
- **RoboLink keys**: throttle, steer, arm_x, arm_y, grip

## Documentation & Specifications
- [`docs/FINAL_PROJECT_SPEC.md`](docs/FINAL_PROJECT_SPEC.md): **Authoritative Final Specification** based on the final BOM (`ROBOREACH BOM - Sheet1.pdf`).
- [`docs/SOFTWARE_ARCHITECTURE.md`](docs/SOFTWARE_ARCHITECTURE.md): Software architecture, data flow, and electrical dependencies.
- [`docs/PROTOTYPE_REFERENCE.md`](docs/PROTOTYPE_REFERENCE.md): Prototype test bench notes, architectural patterns, and deprecation details.

## Project Structure
- `docs/`: Authoritative hardware specifications and reference notes.
- `.agents/skills/roboreach-robot/`: Project-specific development guidelines and hardware constraints.
- `src/`: Application source files (firmware implementation).
- `include/`: Header files, pinouts, and configurations.
- `lib/`: Modular drivers (motor drive, arm control, comms, safety).
- `tests/`: Unit and hardware verification tests:
  - [`tests/robolink_comm_test/`](tests/robolink_comm_test/README.md): RoboLink mobile app communication verification test.

## Build, Upload and Test

### Prerequisites
1. **PlatformIO Core**: `pip install platformio`
2. **ESP32 Board**: Connected to COM14 (or adjust port below)

### PlatformIO Setup
1. Install PlatformIO extension/CLI
2. This project uses `platformio.ini` with:
   - Platform: `pioarduino` (Arduino-ESP32 core 3.x)
   - Framework: `arduino`
   - Monitor speed: `115200`
   - Upload port: `COM14`
   - Library dependencies: `madhephaestus/ESP32Servo`, RoboLink library

### Building
```bash
pio run
```
- Fix any compile errors minimal changes
- The RoboLink library must be available (see lib_deps in platformio.ini)

### Upload
```bash
pio run -t upload
```

### Serial Monitor
```bash
pio device monitor -b 115200
```
- Observe startup messages and control input logging
- Verify: Wi-Fi AP starts, RoboLink connectivity, motor/servo response

### Testing Workflow
1. Upload firmware to ESP32
2. Power cycle or press EN
3. Observe Serial Monitor output (115200 baud)
4. Connect phone to Wi-Fi "RoboReach"
5. Open RoboLink app, ensure Wi-Fi/UDP mode
6. Move joysticks/sliders - verify control values appear in Serial Monitor
7. Test arm movements: shoulder (arm_x), elbow (arm_y), gripper (grip)
8. Test base: throttle (forward/backward), steer (left/right)