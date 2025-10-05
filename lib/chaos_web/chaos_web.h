#ifndef CHAOS_WEB_H
#define CHAOS_WEB_H

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "chaos_core.h"

class ChaosWeb {
private:
    WebServer* server;
    bool isInitialized;
    
    // Route handlers - Updated for WebServer compatibility
    void handleRoot();
    void handleStatus();
    void handleBLEScanStart();
    void handleBLEScanStop();
    void handleBLEJamStart();
    void handleBLEJamStop();
    void handleWiFiScanStart();
    void handleWiFiScanStop();
    void handleWiFiJamStart();
    void handleWiFiJamStop();
    void handleScanResults();
    
    // Utility functions - Updated for WebServer compatibility
    void sendJSONResponse(String json);
    void sendErrorResponse(String error);
    
public:
    ChaosWeb();
    ~ChaosWeb();
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // Server control
    void startServer();
    void stopServer();
    
    // Status
    bool isServerRunning() { return isInitialized && server != nullptr; }
};

// Global instance
extern ChaosWeb chaosWeb;

#endif // CHAOS_WEB_H
