# RoboReach Prototype Reference & Testing Notes

> [!WARNING]
> **PROTOTYPE & TESTING REFERENCE ONLY**  
> The specifications, wiring diagrams, and code snippets in this document reflect earlier test-bench prototypes and experiments.  
> **They MUST NOT override or be copied into the final project design.**  
> The authoritative hardware specification for the final robot is located in [`docs/FINAL_PROJECT_SPEC.md`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/docs/FINAL_PROJECT_SPEC.md), sourced from `ROBOREACH BOM - Sheet1.pdf`.

---

## 1. Prototype vs. Final Hardware Comparison

The following table summarizes the key differences between the earlier prototype / test bench setup and the final production build:

| System Element | Prototype / Bench Test Setup | Final Production Hardware (Authoritative BOM) |
| :--- | :--- | :--- |
| **Drive Motors** | 12V 60 RPM DC motors | **12V 100 RPM Geared DC Motors (×2)** |
| **Motor Driver** | HW-095 (L298N variant, low current) | **BTS7960 High-Power Motor Driver (43A H-Bridge)** |
| **Robotic Arm** | 3-servo prototype (2 × MG950, 1 × SG90) | **5-Actuator Arm (Waist: DS3225 25kg, Shoulder: Annimos 60kg, Elbow: DS3225 25kg, Wrist: DS3225 25kg, End Effector: MG90 Metal)** |
| **Microcontroller** | ESP32 DevKit V1 assumed | **ESP32 (Exact board variant TBD)** |
| **Control Software** | Prototype test commands / raw strings | **RoboLink Mobile Application over Wi-Fi** |
| **Power Pack** | 12.8V 2.2Ah 3S1P 18650 pack (testing only) | **Power system & battery specification TBD** |
| **Transmission** | Direct servo horn / direct drive | **3 × GT2 Open Timing Belts (6mm W, 2mm P, 1m L)** |

---

## 2. Why Prototype Specifications Are Deprecated

1. **Motor Driver Current Limits**:
   - The HW-095 / L298N driver has high internal voltage drops (~2V drop across Darlington transistors) and thermal limits of ~2A peak.
   - The final motors (12V 100RPM) under load or stall on 100mm wheels require the BTS7960 MOSFET H-bridge driver, which supports up to 43A and minimal voltage drop.
2. **Arm Payload & Torque Capacity**:
   - The 3-servo configuration (MG950 / SG90) lacked the reach and payload capacity for practical tasks.
   - The final 5-DOF configuration employs the high-voltage Annimos 60kg digital servo on the primary shoulder lift and DS3225 25kg servos for waist, elbow, and wrist, driven via GT2 timing belts for mechanical advantage.
3. **Power Architecture**:
   - The small 3S1P 18650 prototype battery cannot support the combined stall current of the BTS7960 drive motors, the 60kg Annimos servo, and four 25kg/micro servos without brownout. The final power supply rail architecture is being redesigned and remains TBD.

---

## 3. Preserved Reference Patterns & Architectural Concepts

While specific pinouts and ratings are obsolete, the following design concepts from prototype testing remain valuable architectural references:

### 3.1 Differential Drive Motion Logic
The high-level kinematic logic for 2-wheel differential steering:
- **Forward**: Left Motor Forward + Right Motor Forward
- **Backward**: Left Motor Reverse + Right Motor Reverse
- **Left Turn**: Left Motor Reverse (or Stop) + Right Motor Forward
- **Right Turn**: Left Motor Forward + Right Motor Reverse (or Stop)
- **Speed Adjustment**: Controlled via PWM duty cycles to the driver enable / PWM pins.

### 3.2 Non-Blocking Timing Architecture
Prototypes demonstrated that using `delay()` halts Wi-Fi packets, causing network dropped connections and jerky motor behavior.
- **Rule**: All periodic loops (heartbeats, servo smoothing, motor ramp rates, battery checks) must use `millis()` timestamps.
- **Example Pattern**:
  ```cpp
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL_MS) {
      lastUpdate = now;
      // Perform periodic task (e.g. state estimation, safety checks)
  }
  ```

### 3.3 Modular Code Structure
The project retains the modular directory structure:
- `include/`: Global configuration, pin definitions, and type declarations.
- `lib/`: Modular drivers for base control, arm trajectory, network communication, and safety.
- `src/`: Clean `main.cpp` orchestrating FreeRTOS tasks / Arduino `setup()` and `loop()`.
- `tests/`: Hardware verification sketches (e.g., motor direction test, individual servo sweep test).

---

## 4. Prototype Code Archive Notice

Any test scripts or prototype snippets provided during early development are preserved strictly as reference materials for testing individual peripherals on the bench. Under no circumstances should prototype GPIO constants or driver classes be imported into the final `src/` codebase without verifying against the final wiring diagram and [`docs/FINAL_PROJECT_SPEC.md`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/docs/FINAL_PROJECT_SPEC.md).
