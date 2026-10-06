/*
 * RoboReach - RoboLink Communication Test (Official Library Version)
 * ==================================================================
 * File: tests/robolink_comm_test/robolink_comm_test.ino
 * Purpose: Verify wireless communication between the RoboLink mobile app and ESP32.
 *
 * SAFETY & HARDWARE CONSTRAINTS:
 * - NO motors, servos, or GPIO outputs are controlled in this test.
 * - No specific GPIO pins are configured or assigned.
 * - Compatible with any ESP32 board variant.
 * - Prints all received keys and values (joysticks, buttons, sliders) to Serial Monitor.
 *
 * REQUIREMENTS:
 * - Arduino IDE with ESP32 board package installed.
 * - "RoboLink" library installed:
 *   Available via Arduino Library Manager ("RoboLink" by Sakib Ahmed Shanto)
 *   or GitHub: https://github.com/sakibahmedshanto/RoboLink-Arduino-Library
 */

#include <RoboLink.h>

/* -- Wi-Fi Access Point Configuration --------------------------------- */
// The ESP32 broadcasts this Wi-Fi network; the smartphone connects to it.
const char* AP_SSID     = "RoboReach_Test";
const char* AP_PASSWORD = "password123";  // Minimum 8 characters for WPA2

/* -- Global Objects & State ------------------------------------------- */
RoboLinkWiFi robolink;
unsigned long lastStatusPrint = 0;

/* Callback function: triggered whenever any control value arrives or changes */
void onControlValueReceived(const char* key, int value) {
    Serial.print(F("[RoboLink RX] Key: \""));
    Serial.print(key);
    Serial.print(F("\" | Value: "));
    Serial.println(value);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println(F("\n=============================================="));
    Serial.println(F("    RoboReach: RoboLink Communication Test    "));
    Serial.println(F("=============================================="));
    Serial.println(F("Starting ESP32 Wi-Fi Access Point..."));

    // Start ESP32 in Access Point mode on default UDP port 4210
    if (!robolink.beginAP(AP_SSID, AP_PASSWORD)) {
        Serial.println(F("[ERROR] Failed to start Wi-Fi Access Point!"));
        while (true) {
            delay(1000);
        }
    }

    Serial.println(F("[OK] Wi-Fi Access Point started successfully!"));
    Serial.print(F(" -> Network SSID : ")); Serial.println(AP_SSID);
    Serial.print(F(" -> Password     : ")); Serial.println(AP_PASSWORD);
    Serial.print(F(" -> ESP32 IP     : ")); Serial.println(robolink.localIP());
    Serial.println(F(" -> UDP Port     : 4210 (RoboLink Default)"));
    Serial.println(F("----------------------------------------------"));
    Serial.println(F("NEXT STEPS:"));
    Serial.println(F("1. On your phone, connect to Wi-Fi: RoboReach_Test"));
    Serial.println(F("2. Open the RoboLink mobile application."));
    Serial.println(F("3. Add widgets (joystick, buttons, sliders) with key names."));
    Serial.println(F("4. Move widgets and verify output appears in this Serial Monitor."));
    Serial.println(F("==============================================\n"));

    // Register callback to print every value update
    robolink.onReceive(onControlValueReceived);

    // Diagnostic sensor update interval (sends uptime/heap back to app every 250ms)
    robolink.setSendInterval(250);
}

void loop() {
    // Process incoming UDP packets and trigger callback
    robolink.update();

    // Provide harmless diagnostic telemetry if app has telemetry widgets configured
    robolink.setSensor("uptime_s", (int)(millis() / 1000));
    robolink.setSensor("free_heap_kb", (int)(ESP.getFreeHeap() / 1024));

    // Periodic status summary every 3 seconds
    if (millis() - lastStatusPrint >= 3000) {
        lastStatusPrint = millis();

        int clientCount = robolink.clientCount();
        if (clientCount == 0) {
            Serial.println(F("[STATUS] Waiting for phone to connect to ESP32 Wi-Fi..."));
        } else if (!robolink.hasReceivedData()) {
            Serial.println(F("[STATUS] Phone connected to Wi-Fi. Waiting for RoboLink app data..."));
        } else if (robolink.isTimedOut()) {
            Serial.println(F("[STATUS] App idle or stopped sending packets."));
        } else {
            Serial.print(F("[STATUS] Active link: Receiving packets ("));
            Serial.print(robolink.dataCount());
            Serial.println(F(" active control keys)."));
        }
    }
}
