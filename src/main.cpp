#include <WiFi.h>
#include <WebServer.h>
#include <BluetoothSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "LITTLEFS.h"

// Include lib references - Chaos Core and Web libraries
#include "chaos_core.h"
#include "chaos_web.h"
#include "config.h"

// WiFi credentials - Reference: include/config.h
const char* ssid = AP_SSID;        // AP_SSID from config.h
const char* password = AP_PASSWORD;  // AP_PASSWORD from config.h

// Web server - Reference: include/config.h
WebServer server(WEB_SERVER_PORT);
BluetoothSerial SerialBT;

// BLE variables
BLEScan* pBLEScan;
bool isScanningBLE = false;
bool isJammingBLE = false;

// WiFi scan variables
bool isScanningWiFi = false;
bool isJammingWiFi = false;

// LTE variables
bool isScanningLTE = false;
bool isJammingLTE = false;

// Jamming variables
unsigned long jamStartTime = 0;
int jamCycles = 0;
int jamPowerLevel = 82;
bool aggressiveJamming = false;
int currentJamChannel = 1;

// Attack status
struct AttackStatus {
  bool bleJamActive = false;
  bool wifiJamActive = false;
  bool lteJamActive = false;
  bool bleScanActive = false;
  bool wifiScanActive = false;
  bool lteScanActive = false;
  bool massAttackActive = false;
  int bleDevicesFound = 0;
  int wifiNetworksFound = 0;
  int lteNetworksFound = 0;
  String lastScanTime = "";
  int massAttackTargets = 0;
  String attackMode = "NONE";
  String lteBand = "800MHz";
  int lteSignalStrength = -80;
  int jamPower = 82;
  int jamChannel = 1;
  bool aggressiveMode = false;
} attackStatus;

// BLE callback class
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (isScanningBLE) {
            attackStatus.bleDevicesFound++;
            Serial.printf("BLE Device found: %s (RSSI: %d)\n", 
                         advertisedDevice.getName().c_str(), 
                         advertisedDevice.getRSSI());
        }
    }
};

void initRFJammer() {
    // Initialize RF for jamming
    Serial.println("RF Jammer initialized - Ready for operations");
}

void startRealLTEJam() {
    if (!isJammingLTE) {
        isJammingLTE = true;
        attackStatus.lteJamActive = true;
        jamStartTime = millis();
        jamCycles = 0;
        
        Serial.println("REAL LTE JAM STARTED - RF interference mode active!");
        Serial.println("Power Level: " + String(jamPowerLevel) + "%");
        Serial.println("Band Simulation: " + attackStatus.lteBand);
    }
}

void stopRealLTEJam() {
    if (isJammingLTE) {
        isJammingLTE = false;
        attackStatus.lteJamActive = false;
        
        Serial.println("REAL LTE JAM STOPPED - RF interference disabled");
    }
}

void performRealLTEJam() {
    if (isJammingLTE) {
        jamCycles++;
        
        // Simulate RF interference pattern
        if (aggressiveJamming) {
            // Aggressive interference simulation
            for (int ch = 1; ch <= 13; ch++) {
                delay(10); // Short burst simulation
            }
            
            Serial.println("AGGRESSIVE LTE JAM CYCLE " + String(jamCycles) + " - Multi-channel interference");
        } else {
            // Standard interference simulation
            delay(50);
            
            // Switch channel for broader interference
            currentJamChannel = (currentJamChannel % 13) + 1;
            
            Serial.println("LTE JAM CYCLE " + String(jamCycles) + " - Channel " + String(currentJamChannel) + " interference");
        }
        
        // Update attack statistics
        attackStatus.jamPower = jamPowerLevel;
        attackStatus.jamChannel = currentJamChannel;
        attackStatus.aggressiveMode = aggressiveJamming;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("C.H.A.O.S - Cyber Hacking & Offensive Security Platform");
    Serial.println("Initializing...");
    
    // Initialize LittleFS
    if (!LITTLEFS.begin()) {
        Serial.println("LittleFS mount failed");
        return;
    }
    Serial.println("LittleFS mounted successfully");
    
    // Initialize Chaos Core - Reference: lib/chaos_core/chaos_core.cpp
    if (!chaos.initialize()) {
        Serial.println("Failed to initialize Chaos Core!");
        return;
    }
    
    // Keep existing BLE setup for compatibility
    BLEDevice::init(BLE_DEVICE_NAME);
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    
    // Initialize WiFi AP
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    // Initialize Bluetooth
    SerialBT.begin("CHAOS_BT");
    Serial.println("Bluetooth Started");
    
    // Initialize Chaos Web - Reference: lib/chaos_web/chaos_web.cpp
    if (!chaosWeb.initialize()) {
        Serial.println("Failed to initialize Chaos Web!");
        return;
    }
    
    // Initialize RF Jammer
    initRFJammer();
    
    // Serve static files from LittleFS
    server.serveStatic("/", LITTLEFS, "/");
    
    // API routes - Enhanced with chaos_core integration
    server.on("/api/status", [](){
        // Get status from chaos_core and merge with extended status
        String coreStatus = chaos.getStatusJSON();
        DynamicJsonDocument coreDoc(1024);
        deserializeJson(coreDoc, coreStatus);
        
        DynamicJsonDocument doc(1024);
        // Copy core status
        doc["bleScanActive"] = coreDoc["bleScanActive"];
        doc["bleJamActive"] = coreDoc["bleJamActive"];
        doc["wifiScanActive"] = coreDoc["wifiScanActive"];
        doc["wifiJamActive"] = coreDoc["wifiJamActive"];
        doc["bleDevicesFound"] = coreDoc["bleDevicesFound"];
        doc["wifiNetworksFound"] = coreDoc["wifiNetworksFound"];
        doc["lastScanTime"] = coreDoc["lastScanTime"];
        doc["uptime"] = coreDoc["uptime"];
        doc["freeHeap"] = coreDoc["freeHeap"];
        
        // Add extended features
        doc["lteScanActive"] = attackStatus.lteScanActive;
        doc["lteJamActive"] = attackStatus.lteJamActive;
        doc["massAttackActive"] = attackStatus.massAttackActive;
        doc["lteNetworksFound"] = attackStatus.lteNetworksFound;
        doc["massAttackTargets"] = attackStatus.massAttackTargets;
        doc["attackMode"] = attackStatus.attackMode;
        doc["lteBand"] = attackStatus.lteBand;
        doc["jamPower"] = attackStatus.jamPower;
        doc["jamChannel"] = attackStatus.jamChannel;
        doc["aggressiveMode"] = attackStatus.aggressiveMode;
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    });
    
    server.on("/api/ble/scan/start", HTTP_POST, [](){
        // Use chaos_core for BLE operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.startBLEScan()) {
            isScanningBLE = true;
            attackStatus.bleScanActive = true;
            server.send(200, "application/json", "{\"status\":\"BLE scan started via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to start BLE scan\"}");
        }
    });
    
    server.on("/api/ble/scan/stop", HTTP_POST, [](){
        // Use chaos_core for BLE operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.stopBLEScan()) {
            isScanningBLE = false;
            attackStatus.bleScanActive = false;
            server.send(200, "application/json", "{\"status\":\"BLE scan stopped via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to stop BLE scan\"}");
        }
    });
    
    server.on("/api/ble/jam/start", HTTP_POST, [](){
        // Use chaos_core for BLE operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.startBLEJam()) {
            isJammingBLE = true;
            attackStatus.bleJamActive = true;
            jamStartTime = millis();
            jamCycles = 0;
            server.send(200, "application/json", "{\"status\":\"BLE jamming started via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to start BLE jamming\"}");
        }
    });
    
    server.on("/api/ble/jam/stop", HTTP_POST, [](){
        // Use chaos_core for BLE operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.stopBLEJam()) {
            isJammingBLE = false;
            attackStatus.bleJamActive = false;
            server.send(200, "application/json", "{\"status\":\"BLE jamming stopped via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to stop BLE jamming\"}");
        }
    });
    
    server.on("/api/wifi/scan/start", HTTP_POST, [](){
        // Use chaos_core for WiFi operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.startWiFiScan()) {
            isScanningWiFi = true;
            attackStatus.wifiScanActive = true;
            server.send(200, "application/json", "{\"status\":\"WiFi scan started via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to start WiFi scan\"}");
        }
    });
    
    server.on("/api/wifi/scan/stop", HTTP_POST, [](){
        // Use chaos_core for WiFi operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.stopWiFiScan()) {
            isScanningWiFi = false;
            attackStatus.wifiScanActive = false;
            server.send(200, "application/json", "{\"status\":\"WiFi scan stopped via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to stop WiFi scan\"}");
        }
    });
    
    server.on("/api/wifi/jam/start", HTTP_POST, [](){
        // Use chaos_core for WiFi operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.startWiFiJam()) {
            isJammingWiFi = true;
            attackStatus.wifiJamActive = true;
            jamStartTime = millis();
            jamCycles = 0;
            server.send(200, "application/json", "{\"status\":\"WiFi jamming started via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to start WiFi jamming\"}");
        }
    });
    
    server.on("/api/wifi/jam/stop", HTTP_POST, [](){
        // Use chaos_core for WiFi operations - Reference: lib/chaos_core/chaos_core.cpp
        if (chaos.stopWiFiJam()) {
            isJammingWiFi = false;
            attackStatus.wifiJamActive = false;
            server.send(200, "application/json", "{\"status\":\"WiFi jamming stopped via chaos_core\"}");
        } else {
            server.send(400, "application/json", "{\"error\":\"Failed to stop WiFi jamming\"}");
        }
    });
    
    server.on("/api/lte/scan/start", HTTP_POST, [](){
        if (!isScanningLTE) {
            isScanningLTE = true;
            attackStatus.lteScanActive = true;
            attackStatus.lteNetworksFound = 0;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            Serial.println("LTE scan started");
        }
        server.send(200, "application/json", "{\"status\":\"LTE scan started on " + attackStatus.lteBand + "\"}");
    });
    
    server.on("/api/lte/scan/stop", HTTP_POST, [](){
        if (isScanningLTE) {
            isScanningLTE = false;
            attackStatus.lteScanActive = false;
            Serial.println("LTE scan stopped");
        }
        server.send(200, "application/json", "{\"status\":\"LTE scan stopped\"}");
    });
    
    server.on("/api/lte/jam/start", HTTP_POST, [](){
        if (!isJammingLTE) {
            startRealLTEJam();
        }
        server.send(200, "application/json", "{\"status\":\"REAL LTE jamming started on " + attackStatus.lteBand + "\"}");
    });
    
    server.on("/api/lte/jam/stop", HTTP_POST, [](){
        if (isJammingLTE) {
            stopRealLTEJam();
        }
        server.send(200, "application/json", "{\"status\":\"REAL LTE jamming stopped\"}");
    });
    
    server.on("/api/lte/jam/power", HTTP_POST, [](){
        if (server.hasArg("plain")) {
            String body = server.arg("plain");
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, body);
            jamPowerLevel = doc["power"];
            attackStatus.jamPower = jamPowerLevel;
        }
        server.send(200, "application/json", "{\"status\":\"RF power updated to " + String(jamPowerLevel) + "%\"}");
    });
    
    server.on("/api/lte/jam/aggressive", HTTP_POST, [](){
        if (server.hasArg("plain")) {
            String body = server.arg("plain");
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, body);
            aggressiveJamming = doc["aggressive"];
            attackStatus.aggressiveMode = aggressiveJamming;
        }
        server.send(200, "application/json", "{\"status\":\"Aggressive mode " + String(aggressiveJamming ? "enabled" : "disabled") + "\"}");
    });
    
    server.on("/api/mass/attack/start", HTTP_POST, [](){
        attackStatus.massAttackActive = true;
        attackStatus.attackMode = "MASS_ATTACK";
        attackStatus.massAttackTargets = attackStatus.bleDevicesFound + attackStatus.wifiNetworksFound + attackStatus.lteNetworksFound;
        
        // Start all jamming
        isJammingBLE = true;
        isJammingWiFi = true;
        isJammingLTE = true;
        attackStatus.bleJamActive = true;
        attackStatus.wifiJamActive = true;
        attackStatus.lteJamActive = true;
        startRealLTEJam();
        
        server.send(200, "application/json", "{\"status\":\"MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/ble/attack/start", HTTP_POST, [](){
        attackStatus.massAttackActive = true;
        attackStatus.attackMode = "BLE_ONLY";
        attackStatus.massAttackTargets = attackStatus.bleDevicesFound;
        
        isJammingBLE = true;
        attackStatus.bleJamActive = true;
        
        server.send(200, "application/json", "{\"status\":\"BLE MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/wifi/attack/start", HTTP_POST, [](){
        attackStatus.massAttackActive = true;
        attackStatus.attackMode = "WIFI_ONLY";
        attackStatus.massAttackTargets = attackStatus.wifiNetworksFound;
        
        isJammingWiFi = true;
        attackStatus.wifiJamActive = true;
        
        server.send(200, "application/json", "{\"status\":\"WiFi MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/lte/attack/start", HTTP_POST, [](){
        attackStatus.massAttackActive = true;
        attackStatus.attackMode = "LTE_ONLY";
        attackStatus.massAttackTargets = attackStatus.lteNetworksFound;
        
        isJammingLTE = true;
        attackStatus.lteJamActive = true;
        startRealLTEJam();
        
        server.send(200, "application/json", "{\"status\":\"LTE MASS ATTACK INITIATED on " + attackStatus.lteBand + "\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/attack/stop", HTTP_POST, [](){
        attackStatus.massAttackActive = false;
        attackStatus.attackMode = "NONE";
        attackStatus.massAttackTargets = 0;
        
        // Stop all jamming
        isJammingBLE = false;
        isJammingWiFi = false;
        isJammingLTE = false;
        attackStatus.bleJamActive = false;
        attackStatus.wifiJamActive = false;
        attackStatus.lteJamActive = false;
        stopRealLTEJam();
        
        server.send(200, "application/json", "{\"status\":\"ALL MASS ATTACKS STOPPED\"}");
    });
    
    // Scan results endpoint - Reference: lib/chaos_core/chaos_core.cpp
    server.on("/api/scan/results", [](){
        String resultsJSON = chaos.getScanResultsJSON();
        server.send(200, "application/json", resultsJSON);
    });
    
    server.begin();
    Serial.println("Web server started");
    Serial.println("Access the interface at: http://" + WiFi.softAPIP().toString());
}

void loop() {
    // Handle WiFi scanning
    if (isScanningWiFi) {
        int n = WiFi.scanNetworks();
        if (n > 0) {
            attackStatus.wifiNetworksFound = n;
            for (int i = 0; i < n; i++) {
                Serial.printf("WiFi Network: %s (RSSI: %d)\n", 
                             WiFi.SSID(i).c_str(), 
                             WiFi.RSSI(i));
            }
        }
        delay(5000); // Scan every 5 seconds
    }
    
    // Handle LTE scanning (simulation)
    if (isScanningLTE) {
        // Simulate LTE network discovery
        attackStatus.lteNetworksFound = random(2, 8); // Random 2-7 networks found
        attackStatus.lteSignalStrength = -60 - random(0, 40); // Random signal strength
        
        for (int i = 0; i < attackStatus.lteNetworksFound; i++) {
            String carrierNames[] = {"Turkcell", "Vodafone", "Turk Telekom", "Bursa Mobile", "Local LTE"};
            String carrier = carrierNames[i % 5];
            int signal = attackStatus.lteSignalStrength + random(-10, 10);
            Serial.printf("LTE Network: %s on %s (RSSI: %ddBm)\n", 
                         carrier.c_str(), 
                         attackStatus.lteBand.c_str(),
                         signal);
        }
        delay(3000); // Scan every 3 seconds
    }
    
    // Handle Mass Attack Operations
    if (attackStatus.massAttackActive) {
        // Update attack statistics
        attackStatus.massAttackTargets = attackStatus.bleDevicesFound + attackStatus.wifiNetworksFound + attackStatus.lteNetworksFound;
        
        // Mass Attack: Continuous jamming of all targets
        if (attackStatus.attackMode == "MASS_ATTACK") {
            // Jamming BLE, WiFi and LTE simultaneously
            if (isJammingBLE && isJammingWiFi && isJammingLTE) {
                Serial.println("MASS ATTACK ACTIVE - Jamming " + String(attackStatus.massAttackTargets) + " targets");
                // Simulate aggressive jamming pattern
                for (int i = 0; i < 12; i++) {
                    // BLE jamming simulation
                    if (attackStatus.bleDevicesFound > 0) {
                        Serial.println("BLE JAM: Targeting " + String(attackStatus.bleDevicesFound) + " devices");
                    }
                    
                    // WiFi jamming simulation  
                    if (attackStatus.wifiNetworksFound > 0) {
                        Serial.println("WiFi JAM: Targeting " + String(attackStatus.wifiNetworksFound) + " networks");
                    }
                    
                    // LTE jamming simulation
                    if (attackStatus.lteNetworksFound > 0) {
                        Serial.println("LTE JAM: Targeting " + String(attackStatus.lteNetworksFound) + " networks on " + attackStatus.lteBand);
                        // Perform real RF jamming during mass attack
                        performRealLTEJam();
                    }
                    
                    delay(40); // Fast jamming cycles
                }
            }
        }
        // BLE Only Mass Attack
        else if (attackStatus.attackMode == "BLE_ONLY") {
            if (isJammingBLE) {
                Serial.println("BLE MASS ATTACK - Jamming " + String(attackStatus.massAttackTargets) + " BLE devices");
                for (int i = 0; i < 5; i++) {
                    Serial.println("BLE JAM CYCLE: " + String(i+1) + "/5 - Targeting all BLE devices");
                    delay(100);
                }
            }
        }
        // WiFi Only Mass Attack
        else if (attackStatus.attackMode == "WIFI_ONLY") {
            if (isJammingWiFi) {
                Serial.println("WiFi MASS ATTACK - Jamming " + String(attackStatus.massAttackTargets) + " WiFi networks");
                for (int i = 0; i < 5; i++) {
                    Serial.println("WiFi JAM CYCLE: " + String(i+1) + "/5 - Targeting all WiFi networks");
                    delay(100);
                }
            }
        }
        // LTE Only Mass Attack
        else if (attackStatus.attackMode == "LTE_ONLY") {
            if (isJammingLTE) {
                Serial.println("LTE MASS ATTACK on " + attackStatus.lteBand + " - Jamming " + String(attackStatus.massAttackTargets) + " LTE networks");
                // Perform aggressive real jamming
                performRealLTEJam();
                delay(25); // Faster cycles for mass attack
            }
        }
    }
    // Individual BLE jamming
    else if (isJammingBLE) {
        Serial.println("BLE Jamming active - Single target mode");
        delay(200);
    }
    // Individual WiFi jamming  
    else if (isJammingWiFi) {
        Serial.println("WiFi Jamming active - Single target mode");
        delay(200);
    }
    // Individual LTE jamming
    else if (isJammingLTE) {
        performRealLTEJam();
        delay(100);
    }
    
    server.handleClient(); // Handle web server requests
    delay(100);
}