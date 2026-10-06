# RoboLink to ESP32 Communication Test Guide

This test suite verifies wireless communication between the **RoboLink** mobile application and an **ESP32** microcontroller.

> [!IMPORTANT]
> **SAFETY & ISOLATION**:
> - This test is **purely for communication verification**.
> - **NO motors, servos, or GPIO outputs** are toggled or controlled.
> - Compatible with **any standard ESP32 board**.
> - Does **not** assume any final robot pinouts or hardware specifications.

---

## 1. Test Sketches Available

In [`tests/robolink_comm_test/`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/tests/robolink_comm_test):
1. **[`robolink_comm_test.ino`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/tests/robolink_comm_test/robolink_comm_test.ino)**:  
   Uses the official **RoboLink** Arduino library (`RoboLink.h`). Recommended for standard development.
2. **[`robolink_standalone_test.ino`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/tests/robolink_comm_test/robolink_standalone_test.ino)**:  
   Zero external dependencies (uses only the ESP32 Core's built-in `WiFi.h` and `WiFiUdp.h`). Implements the official RoboLink wire protocol directly (`key:val\n` over UDP port 4210).

---

## 2. Step-by-Step Setup & Upload Instructions

### Step 1: Install ESP32 Board Support in Arduino IDE
1. Open **Arduino IDE**.
2. Go to **File → Preferences** (or `Ctrl + ,`).
3. In **Additional boards manager URLs**, ensure the ESP32 package index URL is present:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Open **Tools → Board → Boards Manager...**, search for `esp32` by **Espressif Systems**, and click **Install**.

### Step 2: Install the RoboLink Library (For `robolink_comm_test.ino`)
*Method A (Arduino Library Manager)*:
1. In Arduino IDE, go to **Sketch → Include Library → Manage Libraries...** (or `Ctrl + Shift + I`).
2. Search for `RoboLink` (by Sakib Ahmed Shanto).
3. Click **Install**.

*Method B (Manual ZIP / Git Clone)*:
- If preferred or if using PlatformIO, clone or download:
  `https://github.com/sakibahmedshanto/RoboLink-Arduino-Library` into your `Arduino/libraries/` directory.

*(Note: If you want to test immediately without installing any external library, open and upload `robolink_standalone_test.ino` instead).*

### Step 3: Select Board & Port
1. Connect your ESP32 board to your computer via USB.
2. Go to **Tools → Board → esp32** and select your board (e.g., `ESP32 Dev Module`, `NodeMCU-32S`, or your specific board).
3. Go to **Tools → Port** and select the active COM port assigned to the ESP32.
4. Set **Upload Speed** to `921600` (or `115200` if upload fails).

### Step 4: Upload the Sketch
1. Open [`tests/robolink_comm_test/robolink_comm_test.ino`](file:///c:/Users/ridhs/OneDrive/Desktop/projects/roboreach/tests/robolink_comm_test/robolink_comm_test.ino) in Arduino IDE.
2. Click **Upload** (`Ctrl + U`).
3. If your ESP32 board requires manual boot mode, hold the **BOOT** button when `Connecting......` appears in the console, then release once flashing begins.

### Step 5: Open Serial Monitor
1. Go to **Tools → Serial Monitor** (or `Ctrl + Shift + M`).
2. Set baud rate to **`115200 baud`**.
3. You will see:
   ```text
   ==============================================
       RoboReach: RoboLink Communication Test    
   ==============================================
   Starting ESP32 Wi-Fi Access Point...
   [OK] Wi-Fi Access Point started successfully!
    -> Network SSID : RoboReach_Test
    -> Password     : password123
    -> ESP32 IP     : 192.168.4.1
    -> UDP Port     : 4210 (RoboLink Default)
   ----------------------------------------------
   ```

---

## 3. Connecting the Phone & RoboLink App

1. **Connect Smartphone Wi-Fi**:
   - On your mobile phone, open **Settings → Wi-Fi**.
   - Connect to the Wi-Fi network: **`RoboReach_Test`**.
   - Enter password: **`password123`**.
   *(Note: Dismiss any "No internet access" prompt on your phone; keep connected to this Wi-Fi).*
2. **Open the RoboLink App**:
   - Open the **RoboLink** app on your phone.
   - Ensure the connection mode is set to **Wi-Fi / UDP**.
   - The default target IP is `192.168.4.1` and Port is `4210`.

---

## 4. How to Verify Joystick Movements & Button Presses

1. In the RoboLink app, create or open a controller layout.
2. Add one or more widgets and assign key names:
   - **Joystick**: e.g., key `steer` (X-axis) and key `throttle` (Y-axis), or default joystick keys.
   - **Buttons**: e.g., key `btn1`, key `btn2`, or key `gripper`.
   - **Slider**: e.g., key `slider1` or `speed`.
3. In the app, switch to **Run / Controller Mode**.
4. **Move the Joystick**:
   - Watch the Serial Monitor. Every movement will stream real-time updates:
     ```text
     [RoboLink RX] Key: "throttle" | Value: 120
     [RoboLink RX] Key: "throttle" | Value: 180
     [RoboLink RX] Key: "steer"    | Value: -45
     ```
5. **Press a Button**:
   - When pressed and released, you will see state transitions:
     ```text
     [RoboLink RX] Key: "btn1" | Value: 1
     [RoboLink RX] Key: "btn1" | Value: 0
     ```
6. **Move a Slider**:
   - Continuous intermediate values (e.g. 0 to 255 or 0 to 180) will print instantaneously.

When you see these lines printing corresponding to your touches on the phone screen, **the RoboLink communication link is 100% verified and functional**!
