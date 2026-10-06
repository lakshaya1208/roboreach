#include "robolink_handler.h"
#include <RoboLink.h>

static RoboLinkWiFi robolink;
static bool initialized = false;

// Cached control values
static int currentThrottle = 0;
static int currentSteer    = 0;
static int currentShoulder = SHOULDER_HOME_DEG;
static int currentElbow    = ELBOW_HOME_DEG;
static int currentGripper  = GRIPPER_HOME_DEG;

void robolinkHandlerInit() {
    Serial.println(F("[COMMS] Initializing RoboLink Wi-Fi AP..."));

    if (!robolink.beginAP(ROBOLINK_WIFI_SSID, ROBOLINK_WIFI_PASS, ROBOLINK_UDP_PORT)) {
        Serial.println(F("[COMMS] ERROR: Failed to launch Wi-Fi AP!"));
        initialized = false;
        return;
    }

    robolink.setTimeoutMs(ROBOLINK_TIMEOUT_MS);
    initialized = true;

    Serial.print(F("[COMMS] RoboReach Wi-Fi AP Started: "));
    Serial.println(ROBOLINK_WIFI_SSID);
    Serial.print(F("[COMMS] IP Address : "));
    Serial.println(robolink.localIP());
    Serial.print(F("[COMMS] UDP Port   : "));
    Serial.println(ROBOLINK_UDP_PORT);
}

void robolinkHandlerUpdate() {
    if (!initialized) return;

    robolink.update();

    if (robolinkIsConnected()) {
        // Read mobile base controls
        currentThrottle = robolink.get(KEY_BASE_THROTTLE, 0);
        currentSteer    = robolink.get(KEY_BASE_STEER, 0);

        // Read 2-DOF arm controls (default to home position if not sent)
        currentShoulder = robolink.get(KEY_ARM_SHOULDER, SHOULDER_HOME_DEG);
        currentElbow    = robolink.get(KEY_ARM_ELBOW,    ELBOW_HOME_DEG);
        currentGripper  = robolink.get(KEY_ARM_GRIPPER,  GRIPPER_HOME_DEG);
    } else {
        // If disconnected or timed out, reset drive throttle/steer to 0
        currentThrottle = 0;
        currentSteer    = 0;
    }
}

bool robolinkIsConnected() {
    if (!initialized) return false;
    return (robolink.hasReceivedData() && !robolink.isTimedOut());
}

int robolinkGetThrottle() { return currentThrottle; }
int robolinkGetSteer()    { return currentSteer; }

int robolinkGetShoulder() { return currentShoulder; }
int robolinkGetElbow()    { return currentElbow; }
int robolinkGetGripper()  { return currentGripper; }