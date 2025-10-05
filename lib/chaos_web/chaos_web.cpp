#include "chaos_web.h"
#include "config.h"

// Global instance
ChaosWeb chaosWeb;

ChaosWeb::ChaosWeb() {
    server = nullptr;
    isInitialized = false;
}

ChaosWeb::~ChaosWeb() {
    shutdown();
}

bool ChaosWeb::initialize() {
    if (isInitialized) {
        return true;
    }
    
    server = new AsyncWebServer(WEB_SERVER_PORT);
    if (!server) {
        Serial.println("[ERROR] Failed to create web server");
        return false;
    }
    
    isInitialized = true;
    Serial.println("[INFO] Web server initialized");
    return true;
}

void ChaosWeb::shutdown() {
    if (server) {
        server->end();
        delete server;
        server = nullptr;
    }
    isInitialized = false;
    Serial.println("[INFO] Web server shutdown");
}

void ChaosWeb::startServer() {
    if (!isInitialized || !server) {
        Serial.println("[ERROR] Cannot start server: not initialized");
        return;
    }
    
    // Main page
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->handleRoot(request);
    });
    
    // API routes
    server->on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->handleStatus(request);
    });
    
    server->on("/api/ble/scan/start", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleBLEScanStart(request);
    });
    
    server->on("/api/ble/scan/stop", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleBLEScanStop(request);
    });
    
    server->on("/api/ble/jam/start", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleBLEJamStart(request);
    });
    
    server->on("/api/ble/jam/stop", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleBLEJamStop(request);
    });
    
    server->on("/api/wifi/scan/start", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleWiFiScanStart(request);
    });
    
    server->on("/api/wifi/scan/stop", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleWiFiScanStop(request);
    });
    
    server->on("/api/wifi/jam/start", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleWiFiJamStart(request);
    });
    
    server->on("/api/wifi/jam/stop", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->handleWiFiJamStop(request);
    });
    
    server->on("/api/scan/results", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->handleScanResults(request);
    });
    
    server->begin();
    Serial.println("[INFO] Web server started on port " + String(WEB_SERVER_PORT));
}

void ChaosWeb::stopServer() {
    if (server) {
        server->end();
        Serial.println("[INFO] Web server stopped");
    }
}

void ChaosWeb::handleRoot(AsyncWebServerRequest *request) {
    // HTML content would be loaded from a separate file or stored as a constant
    // For now, we'll send a simple response
    request->send(200, "text/html", "<h1>C.H.A.O.S Web Interface</h1><p>Interface loaded successfully!</p>");
}

void ChaosWeb::handleStatus(AsyncWebServerRequest *request) {
    String statusJSON = chaos.getStatusJSON();
    sendJSONResponse(request, statusJSON);
}

void ChaosWeb::handleBLEScanStart(AsyncWebServerRequest *request) {
    if (chaos.startBLEScan()) {
        sendJSONResponse(request, "{\"status\":\"BLE scan started\"}");
    } else {
        sendErrorResponse(request, "Failed to start BLE scan");
    }
}

void ChaosWeb::handleBLEScanStop(AsyncWebServerRequest *request) {
    if (chaos.stopBLEScan()) {
        sendJSONResponse(request, "{\"status\":\"BLE scan stopped\"}");
    } else {
        sendErrorResponse(request, "Failed to stop BLE scan");
    }
}

void ChaosWeb::handleBLEJamStart(AsyncWebServerRequest *request) {
    if (chaos.startBLEJam()) {
        sendJSONResponse(request, "{\"status\":\"BLE jamming started\"}");
    } else {
        sendErrorResponse(request, "Failed to start BLE jamming");
    }
}

void ChaosWeb::handleBLEJamStop(AsyncWebServerRequest *request) {
    if (chaos.stopBLEJam()) {
        sendJSONResponse(request, "{\"status\":\"BLE jamming stopped\"}");
    } else {
        sendErrorResponse(request, "Failed to stop BLE jamming");
    }
}

void ChaosWeb::handleWiFiScanStart(AsyncWebServerRequest *request) {
    if (chaos.startWiFiScan()) {
        sendJSONResponse(request, "{\"status\":\"WiFi scan started\"}");
    } else {
        sendErrorResponse(request, "Failed to start WiFi scan");
    }
}

void ChaosWeb::handleWiFiScanStop(AsyncWebServerRequest *request) {
    if (chaos.stopWiFiScan()) {
        sendJSONResponse(request, "{\"status\":\"WiFi scan stopped\"}");
    } else {
        sendErrorResponse(request, "Failed to stop WiFi scan");
    }
}

void ChaosWeb::handleWiFiJamStart(AsyncWebServerRequest *request) {
    if (chaos.startWiFiJam()) {
        sendJSONResponse(request, "{\"status\":\"WiFi jamming started\"}");
    } else {
        sendErrorResponse(request, "Failed to start WiFi jamming");
    }
}

void ChaosWeb::handleWiFiJamStop(AsyncWebServerRequest *request) {
    if (chaos.stopWiFiJam()) {
        sendJSONResponse(request, "{\"status\":\"WiFi jamming stopped\"}");
    } else {
        sendErrorResponse(request, "Failed to stop WiFi jamming");
    }
}

void ChaosWeb::handleScanResults(AsyncWebServerRequest *request) {
    String resultsJSON = chaos.getScanResultsJSON();
    sendJSONResponse(request, resultsJSON);
}

void ChaosWeb::sendJSONResponse(AsyncWebServerRequest *request, String json) {
    request->send(200, "application/json", json);
}

void ChaosWeb::sendErrorResponse(AsyncWebServerRequest *request, String error) {
    String errorJSON = "{\"error\":\"" + error + "\"}";
    request->send(400, "application/json", errorJSON);
}
