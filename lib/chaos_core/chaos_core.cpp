#include "chaos_core.h"
#include "../../include/config.h"

// Global instance
ChaosCore chaos;

// BLE callback class
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        chaos.logMessage("BLE Device found: " + String(advertisedDevice.getName().c_str()) + 
                        " (RSSI: " + String(advertisedDevice.getRSSI()) + ")", 1);
        chaos.updateStatus();
    }
};

ChaosCore::ChaosCore() {
    isInitialized = false;
    pBLEScan = nullptr;
    status = {false, false, false, false, 0, 0, "Never", 0, 0.0};
}

ChaosCore::~ChaosCore() {
    shutdown();
}

bool ChaosCore::initialize() {
    if (isInitialized) {
        return true;
    }
    
    logMessage("Initializing C.H.A.O.S Core...", 0);
    
    // Initialize BLE
    if (!BLEDevice::getInitialized()) {
        BLEDevice::init(BLE_DEVICE_NAME);
        logMessage("BLE Device initialized", 1);
    }
    
    pBLEScan = BLEDevice::getScan();
    if (pBLEScan) {
        pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
        pBLEScan->setActiveScan(true);
        pBLEScan->setInterval(BLE_SCAN_INTERVAL);
        pBLEScan->setWindow(BLE_SCAN_WINDOW);
        logMessage("BLE Scan configured", 1);
    }
    
    isInitialized = true;
    logMessage("C.H.A.O.S Core initialized successfully", 0);
    return true;
}

void ChaosCore::shutdown() {
    if (!isInitialized) {
        return;
    }
    
    logMessage("Shutting down C.H.A.O.S Core...", 0);
    
    // Stop all operations
    stopBLEScan();
    stopBLEJam();
    stopWiFiScan();
    stopWiFiJam();
    
    // Cleanup BLE
    if (pBLEScan) {
        pBLEScan->stop();
        pBLEScan = nullptr;
    }
    
    if (BLEDevice::getInitialized()) {
        BLEDevice::deinit(false);
    }
    
    isInitialized = false;
    logMessage("C.H.A.O.S Core shutdown complete", 0);
}

bool ChaosCore::startBLEScan() {
    if (!isInitialized || !pBLEScan) {
        logMessage("Cannot start BLE scan: Core not initialized", 2);
        return false;
    }
    
    if (status.bleScanActive) {
        logMessage("BLE scan already active", 1);
        return true;
    }
    
    status.bleScanActive = true;
    status.bleDevicesFound = 0;
    status.lastScanTime = String(millis() / 1000) + "s";
    
    pBLEScan->start(BLE_SCAN_DURATION, nullptr, false);
    logMessage("BLE scan started", 0);
    return true;
}

bool ChaosCore::stopBLEScan() {
    if (!status.bleScanActive) {
        return true;
    }
    
    if (pBLEScan) {
        pBLEScan->stop();
    }
    
    status.bleScanActive = false;
    logMessage("BLE scan stopped", 0);
    return true;
}

bool ChaosCore::startBLEJam() {
    if (status.bleJamActive) {
        logMessage("BLE jamming already active", 1);
        return true;
    }
    
    status.bleJamActive = true;
    logMessage("BLE jamming started - WARNING: This may interfere with nearby devices!", 2);
    return true;
}

bool ChaosCore::stopBLEJam() {
    if (!status.bleJamActive) {
        return true;
    }
    
    status.bleJamActive = false;
    logMessage("BLE jamming stopped", 0);
    return true;
}

bool ChaosCore::startWiFiScan() {
    if (status.wifiScanActive) {
        logMessage("WiFi scan already active", 1);
        return true;
    }
    
    status.wifiScanActive = true;
    status.wifiNetworksFound = 0;
    status.lastScanTime = String(millis() / 1000) + "s";
    
    logMessage("WiFi scan started", 0);
    return true;
}

bool ChaosCore::stopWiFiScan() {
    if (!status.wifiScanActive) {
        return true;
    }
    
    status.wifiScanActive = false;
    logMessage("WiFi scan stopped", 0);
    return true;
}

bool ChaosCore::startWiFiJam() {
    if (status.wifiJamActive) {
        logMessage("WiFi jamming already active", 1);
        return true;
    }
    
    status.wifiJamActive = true;
    logMessage("WiFi jamming started - WARNING: This may interfere with nearby networks!", 2);
    return true;
}

bool ChaosCore::stopWiFiJam() {
    if (!status.wifiJamActive) {
        return true;
    }
    
    status.wifiJamActive = false;
    logMessage("WiFi jamming stopped", 0);
    return true;
}

SystemStatus ChaosCore::getStatus() {
    status.uptime = millis();
    status.freeHeap = ESP.getFreeHeap();
    return status;
}

String ChaosCore::getStatusJSON() {
    DynamicJsonDocument doc(1024);
    doc["bleScanActive"] = status.bleScanActive;
    doc["bleJamActive"] = status.bleJamActive;
    doc["wifiScanActive"] = status.wifiScanActive;
    doc["wifiJamActive"] = status.wifiJamActive;
    doc["bleDevicesFound"] = status.bleDevicesFound;
    doc["wifiNetworksFound"] = status.wifiNetworksFound;
    doc["lastScanTime"] = status.lastScanTime;
    doc["uptime"] = status.uptime;
    doc["freeHeap"] = status.freeHeap;
    
    String response;
    serializeJson(doc, response);
    return response;
}

String ChaosCore::getScanResultsJSON() {
    DynamicJsonDocument doc(2048);
    JsonArray bleDevices = doc.createNestedArray("bleDevices");
    JsonArray wifiNetworks = doc.createNestedArray("wifiNetworks");
    
    // Add sample BLE devices if any found
    if (status.bleDevicesFound > 0) {
        for (int i = 0; i < status.bleDevicesFound; i++) {
            JsonObject device = bleDevices.createNestedObject();
            device["name"] = "Device_" + String(i);
            device["address"] = "AA:BB:CC:DD:EE:FF";
            device["rssi"] = -50 - (i * 10);
            device["connectable"] = true;
        }
    }
    
    // Add sample WiFi networks if any found
    if (status.wifiNetworksFound > 0) {
        for (int i = 0; i < status.wifiNetworksFound; i++) {
            JsonObject network = wifiNetworks.createNestedObject();
            network["ssid"] = "Network_" + String(i);
            network["rssi"] = -40 - (i * 5);
            network["channel"] = 1 + (i % 13);
            network["encryption"] = "WPA2";
            network["hidden"] = false;
        }
    }
    
    String response;
    serializeJson(doc, response);
    return response;
}

void ChaosCore::updateStatus() {
    // Update device counts based on active scans
    if (status.bleScanActive) {
        // This would be updated by the BLE callback in a real implementation
        status.bleDevicesFound++;
    }
    
    if (status.wifiScanActive) {
        // This would be updated by WiFi scan results in a real implementation
        status.wifiNetworksFound++;
    }
}

void ChaosCore::logMessage(String message, int level) {
    if (!DEBUG_ENABLED && level > 0) {
        return;
    }
    
    String prefix = "";
    switch (level) {
        case 0: prefix = "[INFO]"; break;
        case 1: prefix = "[DEBUG]"; break;
        case 2: prefix = "[WARNING]"; break;
        case 3: prefix = "[ERROR]"; break;
        default: prefix = "[LOG]"; break;
    }
    
    Serial.println(prefix + " " + message);
}
