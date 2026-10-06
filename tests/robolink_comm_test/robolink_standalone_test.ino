/*
 * RoboReach - RoboLink Communication Test (Zero-Dependency Standalone Version)
 * ============================================================================
 * File: tests/robolink_comm_test/robolink_standalone_test.ino
 * Purpose: Verify RoboLink-to-ESP32 UDP communication without requiring any
 *          external library (uses only ESP32 built-in WiFi.h and WiFiUdp.h).
 *
 * PROTOCOL IMPLEMENTED:
 * - Official RoboLink UDP Wire Format: "key1:val1,key2:val2,...,keyN:valN\n"
 * - Default UDP Port: 4210
 * - Hardware: Any ESP32 board
 *
 * SAFETY & HARDWARE CONSTRAINTS:
 * - NO motors, servos, or GPIO outputs are controlled in this test.
 * - Prints all received control data to the Serial Monitor.
 */

#include <WiFi.h>
#include <WiFiUdp.h>

/* -- Wi-Fi Access Point Configuration --------------------------------- */
const char* AP_SSID      = "RoboReach_Test";
const char* AP_PASSWORD  = "password123";  // Minimum 8 characters for WPA2
const int   UDP_PORT     = 4210;
const unsigned long TIMEOUT_MS = 500;

/* -- Key-Value Internal Buffer ---------------------------------------- */
#define MAX_KEYS  64
#define KEY_LEN   24

struct KeyValue {
    char key[KEY_LEN + 1];
    int  value;
};

static KeyValue      store[MAX_KEYS];
static int           storeCount  = 0;
static unsigned long lastRxTime  = 0;
static IPAddress     remoteIP;
static uint16_t      remotePort  = 0;
static bool          hasRemote   = false;
static WiFiUDP       udp;

/* Check if active packet received within timeout */
bool isConnected() {
    return (lastRxTime > 0 && (millis() - lastRxTime <= TIMEOUT_MS));
}

/* Internal helpers: insert or update key */
static void upsertKey(const char* key, int val) {
    for (int i = 0; i < storeCount; i++) {
        if (strcmp(store[i].key, key) == 0) {
            if (store[i].value != val) {
                store[i].value = val;
                Serial.print(F("[RoboLink RX] Key: \""));
                Serial.print(key);
                Serial.print(F("\" => Value: "));
                Serial.println(val);
            }
            return;
        }
    }
    if (storeCount < MAX_KEYS) {
        strncpy(store[storeCount].key, key, KEY_LEN);
        store[storeCount].key[KEY_LEN] = '\0';
        store[storeCount].value = val;
        Serial.print(F("[RoboLink RX - New Key] \""));
        Serial.print(key);
        Serial.print(F("\" => Initial Value: "));
        Serial.println(val);
        storeCount++;
    }
}

/* Parse incoming line: key1:val1,key2:val2\n */
static void parseLine(char* line) {
    char* p = line;
    while (*p) {
        char* colon = strchr(p, ':');
        if (!colon) break;
        int klen = colon - p;
        if (klen > 0 && klen <= KEY_LEN) {
            char key[KEY_LEN + 1];
            memcpy(key, p, klen);
            key[klen] = '\0';
            upsertKey(key, (int)atol(colon + 1));
        }
        char* comma = strchr(colon + 1, ',');
        if (!comma) break;
        p = comma + 1;
    }
    lastRxTime = millis();
}

/* Drain UDP packets */
static uint8_t packetBuffer[513];
static void readUDPPackets() {
    int packetSize;
    while ((packetSize = udp.parsePacket()) > 0) {
        int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
        if (len <= 0) continue;
        packetBuffer[len] = '\0';
        remoteIP   = udp.remoteIP();
        remotePort = udp.remotePort();
        hasRemote  = true;

        // Split packet lines if multiple messages arrived
        char* start = (char*)packetBuffer;
        char* newlinePtr;
        while ((newlinePtr = strchr(start, '\n')) != nullptr) {
            *newlinePtr = '\0';
            if (newlinePtr > start) {
                parseLine(start);
            }
            start = newlinePtr + 1;
        }
        if (*start) {
            parseLine(start);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println(F("\n=============================================="));
    Serial.println(F(" RoboReach: RoboLink Test (Standalone UDP)    "));
    Serial.println(F("=============================================="));

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    delay(100);

    udp.begin(UDP_PORT);

    Serial.println(F("[OK] ESP32 Wi-Fi Access Point and UDP ready!"));
    Serial.print(F(" -> Network SSID : ")); Serial.println(AP_SSID);
    Serial.print(F(" -> Password     : ")); Serial.println(AP_PASSWORD);
    Serial.print(F(" -> ESP32 IP     : ")); Serial.println(WiFi.softAPIP());
    Serial.print(F(" -> UDP Port     : ")); Serial.println(UDP_PORT);
    Serial.println(F("----------------------------------------------"));
    Serial.println(F("INSTRUCTIONS:"));
    Serial.println(F("1. Connect phone to Wi-Fi: RoboReach_Test"));
    Serial.println(F("2. Open RoboLink app, select Wi-Fi / UDP (port 4210)."));
    Serial.println(F("3. Move controls on phone; values will print below."));
    Serial.println(F("==============================================\n"));
}

void loop() {
    // Read incoming UDP packets
    readUDPPackets();

    // Periodic heartbeat on Serial Monitor every 3 seconds
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus >= 3000) {
        lastStatus = millis();

        int stations = (int)WiFi.softAPgetStationNum();
        if (stations == 0) {
            Serial.println(F("[STATUS] Waiting for phone to connect to Wi-Fi..."));
        } else if (lastRxTime == 0) {
            Serial.println(F("[STATUS] Phone connected to Wi-Fi. Waiting for RoboLink app data..."));
        } else if (!isConnected()) {
            Serial.println(F("[STATUS] Communication timed out (app idle or closed)."));
        } else {
            Serial.println(F("--- [ACTIVE CONTROLS SNAPSHOT] ---"));
            for (int i = 0; i < storeCount; i++) {
                Serial.print(F("  "));
                Serial.print(store[i].key);
                Serial.print(F(" = "));
                Serial.println(store[i].value);
            }
        }
    }
}
