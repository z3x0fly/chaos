#ifndef CHAOS_WEB_H
#define CHAOS_WEB_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "chaos_core.h"

class ChaosWeb {
private:
    AsyncWebServer* server;
    bool isInitialized;
    
    // Route handlers
    void handleRoot(AsyncWebServerRequest *request);
    void handleStatus(AsyncWebServerRequest *request);
    void handleBLEScanStart(AsyncWebServerRequest *request);
    void handleBLEScanStop(AsyncWebServerRequest *request);
    void handleBLEJamStart(AsyncWebServerRequest *request);
    void handleBLEJamStop(AsyncWebServerRequest *request);
    void handleWiFiScanStart(AsyncWebServerRequest *request);
    void handleWiFiScanStop(AsyncWebServerRequest *request);
    void handleWiFiJamStart(AsyncWebServerRequest *request);
    void handleWiFiJamStop(AsyncWebServerRequest *request);
    void handleScanResults(AsyncWebServerRequest *request);
    
    // Utility functions
    void sendJSONResponse(AsyncWebServerRequest *request, String json);
    void sendErrorResponse(AsyncWebServerRequest *request, String error);
    
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
