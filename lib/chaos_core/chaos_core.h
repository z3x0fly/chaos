#ifndef CHAOS_CORE_H
#define CHAOS_CORE_H

#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>

// Forward declarations
class ChaosCore;

// Device structure for BLE devices
struct BLEDeviceInfo {
    String name;
    String address;
    int rssi;
    bool isConnectable;
    uint8_t* manufacturerData;
    size_t manufacturerDataLength;
};

// Network structure for WiFi networks
struct WiFiNetworkInfo {
    String ssid;
    int rssi;
    int channel;
    String encryption;
    bool isHidden;
};

// System status structure
struct SystemStatus {
    bool bleScanActive;
    bool bleJamActive;
    bool wifiScanActive;
    bool wifiJamActive;
    int bleDevicesFound;
    int wifiNetworksFound;
    String lastScanTime;
    unsigned long uptime;
    float freeHeap;
};

// Core class for C.H.A.O.S platform
class ChaosCore {
private:
    BLEScan* pBLEScan;
    bool isInitialized;
    SystemStatus status;
    
    // Callback for BLE scan results
    static void onBLEScanResult(BLEAdvertisedDevice advertisedDevice);
    
public:
    ChaosCore();
    ~ChaosCore();
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // BLE Operations
    bool startBLEScan();
    bool stopBLEScan();
    bool startBLEJam();
    bool stopBLEJam();
    
    // WiFi Operations
    bool startWiFiScan();
    bool stopWiFiScan();
    bool startWiFiJam();
    bool stopWiFiJam();
    
    // Status and Results
    SystemStatus getStatus();
    String getStatusJSON();
    String getScanResultsJSON();
    
    // Utility functions
    void updateStatus();
    void logMessage(String message, int level = 0);
    
    // Getters
    bool isBLEActive() { return status.bleScanActive || status.bleJamActive; }
    bool isWiFiActive() { return status.wifiScanActive || status.wifiJamActive; }
};

// Global instance
extern ChaosCore chaos;

#endif // CHAOS_CORE_H
