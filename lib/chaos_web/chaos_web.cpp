#include "chaos_web.h"
#include "../../include/config.h"

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
    
    server = new WebServer(WEB_SERVER_PORT);
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
    server->on("/", HTTP_GET, [this]() {
        this->handleRoot();
    });
    
    // API routes
    server->on("/api/status", HTTP_GET, [this]() {
        this->handleStatus();
    });
    
    server->on("/api/ble/scan/start", HTTP_POST, [this]() {
        this->handleBLEScanStart();
    });
    
    server->on("/api/ble/scan/stop", HTTP_POST, [this]() {
        this->handleBLEScanStop();
    });
    
    server->on("/api/ble/jam/start", HTTP_POST, [this]() {
        this->handleBLEJamStart();
    });
    
    server->on("/api/ble/jam/stop", HTTP_POST, [this]() {
        this->handleBLEJamStop();
    });
    
    server->on("/api/wifi/scan/start", HTTP_POST, [this]() {
        this->handleWiFiScanStart();
    });
    
    server->on("/api/wifi/scan/stop", HTTP_POST, [this]() {
        this->handleWiFiScanStop();
    });
    
    server->on("/api/wifi/jam/start", HTTP_POST, [this]() {
        this->handleWiFiJamStart();
    });
    
    server->on("/api/wifi/jam/stop", HTTP_POST, [this]() {
        this->handleWiFiJamStop();
    });
    
    server->on("/api/scan/results", HTTP_GET, [this]() {
        this->handleScanResults();
    });
    
    server->begin();
    Serial.println("[INFO] Web server started on port " + String(WEB_SERVER_PORT));
}

void ChaosWeb::stopServer() {
    if (server) {
        Serial.println("[INFO] Web server stopped");
    }
}

void ChaosWeb::handleRoot() {
    // HTML content would be loaded from a separate file or stored as a constant
    // For now, we'll send a simple response
    server->send(200, "text/html", "<h1>C.H.A.O.S Web Interface</h1><p>Interface loaded successfully!</p>");
}

void ChaosWeb::handleStatus() {
    String statusJSON = chaos.getStatusJSON();
    sendJSONResponse(statusJSON);
}

void ChaosWeb::handleBLEScanStart() {
    if (chaos.startBLEScan()) {
        sendJSONResponse("{\"status\":\"BLE scan started\"}");
    } else {
        sendErrorResponse("Failed to start BLE scan");
    }
}

void ChaosWeb::handleBLEScanStop() {
    if (chaos.stopBLEScan()) {
        sendJSONResponse("{\"status\":\"BLE scan stopped\"}");
    } else {
        sendErrorResponse("Failed to stop BLE scan");
    }
}

void ChaosWeb::handleBLEJamStart() {
    if (chaos.startBLEJam()) {
        sendJSONResponse("{\"status\":\"BLE jamming started\"}");
    } else {
        sendErrorResponse("Failed to start BLE jamming");
    }
}

void ChaosWeb::handleBLEJamStop() {
    if (chaos.stopBLEJam()) {
        sendJSONResponse("{\"status\":\"BLE jamming stopped\"}");
    } else {
        sendErrorResponse("Failed to stop BLE jamming");
    }
}

void ChaosWeb::handleWiFiScanStart() {
    if (chaos.startWiFiScan()) {
        sendJSONResponse("{\"status\":\"WiFi scan started\"}");
    } else {
        sendErrorResponse("Failed to start WiFi scan");
    }
}

void ChaosWeb::handleWiFiScanStop() {
    if (chaos.stopWiFiScan()) {
        sendJSONResponse("{\"status\":\"WiFi scan stopped\"}");
    } else {
        sendErrorResponse("Failed to stop WiFi scan");
    }
}

void ChaosWeb::handleWiFiJamStart() {
    if (chaos.startWiFiJam()) {
        sendJSONResponse("{\"status\":\"WiFi jamming started\"}");
    } else {
        sendErrorResponse("Failed to start WiFi jamming");
    }
}

void ChaosWeb::handleWiFiJamStop() {
    if (chaos.stopWiFiJam()) {
        sendJSONResponse("{\"status\":\"WiFi jamming stopped\"}");
    } else {
        sendErrorResponse("Failed to stop WiFi jamming");
    }
}

void ChaosWeb::handleScanResults() {
    String resultsJSON = chaos.getScanResultsJSON();
    sendJSONResponse(resultsJSON);
}

void ChaosWeb::sendJSONResponse(String json) {
    server->send(200, "application/json", json);
}

void ChaosWeb::sendErrorResponse(String error) {
    String errorJSON = "{\"error\":\"" + error + "\"}";
    server->send(400, "application/json", errorJSON);
}
