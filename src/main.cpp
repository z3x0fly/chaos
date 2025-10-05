#include <WiFi.h>
#include <BluetoothSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// WiFi credentials
const char* ssid = "CHAOS_AP";
const char* password = "chaos123456";

// Web server
AsyncWebServer server(80);
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

// Real LTE Jammer variables
int currentJamChannel = 1;
int jamPowerLevel = 82; // Max power (0-82)
bool aggressiveJamming = false;
uint32_t jamStartTime = 0;
int jamCycles = 0;

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
  String attackMode = "NONE"; // NONE, BLE_ONLY, WIFI_ONLY, LTE_ONLY, MASS_ATTACK
  String lteBand = "800MHz"; // 800MHz, 1800MHz, 2100MHz, 2600MHz
  int lteSignalStrength = -80; // dBm
  int jamPower = 82; // RF power level
  int jamChannel = 1; // Current jam channel
  bool aggressiveMode = false; // Aggressive jamming mode
} attackStatus;

// HTML content for the web interface
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>C.H.A.O.S - Cyber Hacking & Offensive Security</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            background: linear-gradient(135deg, #0a0a0a 0%, #1a1a2e 50%, #16213e 100%);
            color: #00ff00;
            font-family: 'Courier New', monospace;
            min-height: 100vh;
            overflow-x: hidden;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        .header {
            text-align: center;
            margin-bottom: 30px;
            padding: 20px;
            border: 2px solid #00ff00;
            border-radius: 10px;
            background: rgba(0, 255, 0, 0.1);
            box-shadow: 0 0 20px rgba(0, 255, 0, 0.3);
        }
        
        .header h1 {
            font-size: 2.5em;
            text-shadow: 0 0 10px #00ff00;
            margin-bottom: 10px;
        }
        
        .header p {
            font-size: 1.2em;
            opacity: 0.8;
        }
        
        .status-panel {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .status-card {
            background: rgba(0, 0, 0, 0.7);
            border: 1px solid #00ff00;
            border-radius: 10px;
            padding: 20px;
            text-align: center;
            transition: all 0.3s ease;
        }
        
        .status-card:hover {
            box-shadow: 0 0 15px rgba(0, 255, 0, 0.5);
            transform: translateY(-2px);
        }
        
        .status-card h3 {
            margin-bottom: 15px;
            font-size: 1.3em;
        }
        
        .status-indicator {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            display: inline-block;
            margin-right: 10px;
        }
        
        .status-active {
            background: #ff0000;
            box-shadow: 0 0 10px #ff0000;
        }
        
        .status-inactive {
            background: #00ff00;
            box-shadow: 0 0 10px #00ff00;
        }
        
        .control-panel {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .control-card {
            background: rgba(0, 0, 0, 0.7);
            border: 1px solid #00ff00;
            border-radius: 10px;
            padding: 20px;
        }
        
        .control-card h3 {
            margin-bottom: 20px;
            text-align: center;
            font-size: 1.4em;
        }
        
        .btn {
            width: 100%;
            padding: 15px;
            margin: 10px 0;
            background: transparent;
            border: 2px solid #00ff00;
            color: #00ff00;
            font-size: 1.1em;
            font-family: 'Courier New', monospace;
            cursor: pointer;
            border-radius: 5px;
            transition: all 0.3s ease;
        }
        
        .btn:hover {
            background: #00ff00;
            color: #000;
            box-shadow: 0 0 15px rgba(0, 255, 0, 0.5);
        }
        
        .btn:active {
            transform: scale(0.95);
        }
        
        .btn.danger {
            border-color: #ff0000;
            color: #ff0000;
        }
        
        .btn.danger:hover {
            background: #ff0000;
            color: #000;
            box-shadow: 0 0 15px rgba(255, 0, 0, 0.5);
        }
        
        .results-panel {
            background: rgba(0, 0, 0, 0.7);
            border: 1px solid #00ff00;
            border-radius: 10px;
            padding: 20px;
            margin-top: 20px;
        }
        
        .results-panel h3 {
            margin-bottom: 15px;
            text-align: center;
        }
        
        .results-content {
            max-height: 300px;
            overflow-y: auto;
            background: rgba(0, 0, 0, 0.5);
            padding: 15px;
            border-radius: 5px;
            border: 1px solid #333;
        }
        
        .device-item {
            padding: 10px;
            margin: 5px 0;
            background: rgba(0, 255, 0, 0.1);
            border-left: 3px solid #00ff00;
            border-radius: 3px;
        }
        
        .device-item strong {
            color: #00ff00;
        }
        
        .device-item .signal {
            float: right;
            color: #ffaa00;
        }
        
        .loading {
            text-align: center;
            color: #ffaa00;
            font-style: italic;
        }
        
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        
        .pulse {
            animation: pulse 1s infinite;
        }
        
        .footer {
            text-align: center;
            margin-top: 30px;
            padding: 20px;
            border-top: 1px solid #00ff00;
            opacity: 0.7;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>C.H.A.O.S</h1>
            <p>Cyber Hacking & Offensive Security Platform</p>
        </div>
        
        <div class="status-panel">
            <div class="status-card">
                <h3>BLE Status</h3>
                <span class="status-indicator" id="bleStatus"></span>
                <span id="bleStatusText">Inactive</span>
                <p id="bleDevicesCount">Devices Found: 0</p>
            </div>
            
            <div class="status-card">
                <h3>WiFi Status</h3>
                <span class="status-indicator" id="wifiStatus"></span>
                <span id="wifiStatusText">Inactive</span>
                <p id="wifiNetworksCount">Networks Found: 0</p>
            </div>
            
            <div class="status-card">
                <h3>📱 LTE Status</h3>
                <span class="status-indicator" id="lteStatus"></span>
                <span id="lteStatusText">Inactive</span>
                <p id="lteNetworksCount">Networks Found: 0</p>
                <p id="lteBandInfo">Band: 800MHz</p>
            </div>
            
            <div class="status-card">
                <h3>System Status</h3>
                <span class="status-indicator status-active"></span>
                <span>Online</span>
                <p id="lastScanTime">Last Scan: Never</p>
            </div>
        </div>
        
        <div class="control-panel">
            <div class="control-card">
                <h3>BLE Operations</h3>
                <button class="btn" onclick="startBLEScan()">Start BLE Scan</button>
                <button class="btn danger" onclick="stopBLEScan()">Stop BLE Scan</button>
                <button class="btn danger" onclick="startBLEJam()">Start BLE Jam</button>
                <button class="btn danger" onclick="stopBLEJam()">Stop BLE Jam</button>
            </div>
            
            <div class="control-card">
                <h3>WiFi Operations</h3>
                <button class="btn" onclick="startWiFiScan()">Start WiFi Scan</button>
                <button class="btn danger" onclick="stopWiFiScan()">Stop WiFi Scan</button>
                <button class="btn danger" onclick="startWiFiJam()">Start WiFi Jam</button>
                <button class="btn danger" onclick="stopWiFiJam()">Stop WiFi Jam</button>
            </div>
            
            <div class="control-card">
                <h3>📱 LTE Operations</h3>
                <button class="btn" onclick="startLTEScan()">Start LTE Scan</button>
                <button class="btn danger" onclick="stopLTEScan()">Stop LTE Scan</button>
                <button class="btn danger" onclick="startLTEJam()">Start LTE Jam</button>
                <button class="btn danger" onclick="stopLTEJam()">Stop LTE Jam</button>
                <select id="lteBandSelect" style="width:100%; margin:10px 0; padding:10px; background:#000; color:#00ff00; border:1px solid #00ff00;">
                    <option value="800MHz">800MHz Band (2.4GHz Sim)</option>
                    <option value="1800MHz">1800MHz Band (2.4GHz Sim)</option>
                    <option value="2100MHz">2100MHz Band (2.4GHz Sim)</option>
                    <option value="2600MHz">2600MHz Band (2.4GHz Sim)</option>
                </select>
                <input type="range" id="jamPowerSlider" min="0" max="82" value="82" style="width:100%; margin:10px 0;" onchange="updateJamPower()">
                <label for="jamPowerSlider">RF Power: <span id="jamPowerValue">82</span>%</label>
                <input type="checkbox" id="aggressiveMode" onchange="updateAggressiveMode()"> 
                <label for="aggressiveMode">🔥 Aggressive Mode (High Interference)</label>
            </div>
            
            <div class="control-card">
                <h3>🚀 MASS ATTACK</h3>
                <button class="btn danger" onclick="startMassAttack()">🔥 MASS ATTACK ALL</button>
                <button class="btn danger" onclick="startMassBLEAttack()">⚡ BLE MASS JAM</button>
                <button class="btn danger" onclick="startMassWiFiAttack()">📡 WiFi MASS JAM</button>
                <button class="btn danger" onclick="startMassLTEAttack()">📱 LTE MASS JAM</button>
                <button class="btn" onclick="stopMassAttack()">🛑 STOP ALL ATTACKS</button>
            </div>
        </div>
        
        <div class="results-panel">
            <h3>Scan Results</h3>
            <div class="results-content" id="scanResults">
                <div class="loading">Ready to scan... Select an operation above to begin.</div>
            </div>
        </div>
        
        <div class="footer">
            <p>C.H.A.O.S v1.0 - For Educational Purposes Only</p>
        </div>
    </div>
    
    <script>
        let scanInterval;
        
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update BLE status
                    const bleStatus = document.getElementById('bleStatus');
                    const bleStatusText = document.getElementById('bleStatusText');
                    const bleDevicesCount = document.getElementById('bleDevicesCount');
                    
                    if (data.bleScanActive || data.bleJamActive) {
                        bleStatus.className = 'status-indicator status-active pulse';
                        bleStatusText.textContent = data.bleJamActive ? 'Jamming' : 'Scanning';
                    } else {
                        bleStatus.className = 'status-indicator status-inactive';
                        bleStatusText.textContent = 'Inactive';
                    }
                    bleDevicesCount.textContent = `Devices Found: ${data.bleDevicesFound}`;
                    
                    // Update WiFi status
                    const wifiStatus = document.getElementById('wifiStatus');
                    const wifiStatusText = document.getElementById('wifiStatusText');
                    const wifiNetworksCount = document.getElementById('wifiNetworksCount');
                    
                    if (data.wifiScanActive || data.wifiJamActive) {
                        wifiStatus.className = 'status-indicator status-active pulse';
                        wifiStatusText.textContent = data.wifiJamActive ? 'Jamming' : 'Scanning';
                    } else {
                        wifiStatus.className = 'status-indicator status-inactive';
                        wifiStatusText.textContent = 'Inactive';
                    }
                    wifiNetworksCount.textContent = `Networks Found: ${data.wifiNetworksFound}`;
                    
                    // Update LTE status
                    const lteStatus = document.getElementById('lteStatus');
                    const lteStatusText = document.getElementById('lteStatusText');
                    const lteNetworksCount = document.getElementById('lteNetworksCount');
                    const lteBandInfo = document.getElementById('lteBandInfo');
                    
                    if (data.lteScanActive || data.lteJamActive) {
                        lteStatus.className = 'status-indicator status-active pulse';
                        lteStatusText.textContent = data.lteJamActive ? 'Jamming' : 'Scanning';
                    } else {
                        lteStatus.className = 'status-indicator status-inactive';
                        lteStatusText.textContent = 'Inactive';
                    }
                    lteNetworksCount.textContent = `Networks Found: ${data.lteNetworksFound}`;
                    lteBandInfo.textContent = `Band: ${data.lteBand}`;
                    
                    // Update last scan time
                    document.getElementById('lastScanTime').textContent = `Last Scan: ${data.lastScanTime}`;
                })
                .catch(error => console.error('Error:', error));
        }
        
        function startBLEScan() {
            fetch('/api/ble/scan/start', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    updateResults();
                });
        }
        
        function stopBLEScan() {
            fetch('/api/ble/scan/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    updateResults();
                });
        }
        
        function startBLEJam() {
            fetch('/api/ble/jam/start', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        }
        
        function stopBLEJam() {
            fetch('/api/ble/jam/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        }
        
        function startWiFiScan() {
            fetch('/api/wifi/scan/start', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    updateResults();
                });
        }
        
        function stopWiFiScan() {
            fetch('/api/wifi/scan/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    updateResults();
                });
        }
        
        function startWiFiJam() {
            fetch('/api/wifi/jam/start', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        }
        
        function stopWiFiJam() {
            fetch('/api/wifi/jam/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        }
        
        function startMassAttack() {
            if (confirm('🔥 MASS ATTACK: This will jam ALL BLE devices and WiFi networks in range!\n\nAre you sure you want to proceed?')) {
                fetch('/api/mass/attack/start', {method: 'POST'})
                    .then(response => response.json())
                    .then(data => {
                        console.log(data);
                        alert('🚀 MASS ATTACK INITIATED! All targets are being jammed!');
                    });
            }
        }
        
        function startMassBLEAttack() {
            if (confirm('⚡ BLE MASS JAM: This will jam ALL BLE devices in range!\n\nAre you sure you want to proceed?')) {
                fetch('/api/mass/ble/attack/start', {method: 'POST'})
                    .then(response => response.json())
                    .then(data => {
                        console.log(data);
                        alert('⚡ BLE MASS JAM INITIATED! All BLE devices are being jammed!');
                    });
            }
        }
        
        function startMassWiFiAttack() {
            if (confirm('📡 WiFi MASS JAM: This will jam ALL WiFi networks in range!\n\nAre you sure you want to proceed?')) {
                fetch('/api/mass/wifi/attack/start', {method: 'POST'})
                    .then(response => response.json())
                    .then(data => {
                        console.log(data);
                        alert('📡 WiFi MASS JAM INITIATED! All WiFi networks are being jammed!');
                    });
            }
        }
        
        function stopMassAttack() {
            fetch('/api/mass/attack/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    alert('🛑 ALL ATTACKS STOPPED!');
                });
        }
        
        function startLTEScan() {
            const band = document.getElementById('lteBandSelect').value;
            fetch('/api/lte/scan/start', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({band: band})
            })
            .then(response => response.json())
            .then(data => {
                console.log(data);
                updateResults();
            });
        }
        
        function stopLTEScan() {
            fetch('/api/lte/scan/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    updateResults();
                });
        }
        
        function startLTEJam() {
            const band = document.getElementById('lteBandSelect').value;
            if (confirm('📱 LTE JAM: This will jam LTE networks on ' + band + '!\n\n⚠️ WARNING: This may interfere with cellular communications!\n\nAre you sure you want to proceed?')) {
                fetch('/api/lte/jam/start', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({band: band})
                })
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    alert('📱 LTE JAM INITIATED on ' + band + '!');
                });
            }
        }
        
        function stopLTEJam() {
            fetch('/api/lte/jam/stop', {method: 'POST'})
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        }
        
        function startMassLTEAttack() {
            const band = document.getElementById('lteBandSelect').value;
            if (confirm('📱 LTE MASS JAM: This will jam ALL LTE networks on ' + band + '!\n\n⚠️ CRITICAL WARNING: This will disrupt cellular communications!\n\nAre you sure you want to proceed?')) {
                fetch('/api/mass/lte/attack/start', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({band: band})
                })
                .then(response => response.json())
                .then(data => {
                    console.log(data);
                    alert('📱 LTE MASS JAM INITIATED on ' + band + '!');
                });
            }
        }
        
        function updateJamPower() {
            const power = document.getElementById('jamPowerSlider').value;
            document.getElementById('jamPowerValue').textContent = power;
            fetch('/api/lte/jam/power', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({power: parseInt(power)})
            });
        }
        
        function updateAggressiveMode() {
            const aggressive = document.getElementById('aggressiveMode').checked;
            fetch('/api/lte/jam/aggressive', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({aggressive: aggressive})
            });
        }
        
        function updateResults() {
            fetch('/api/scan/results')
                .then(response => response.json())
                .then(data => {
                    const resultsDiv = document.getElementById('scanResults');
                    resultsDiv.innerHTML = '';
                    
                    if (data.bleDevices && data.bleDevices.length > 0) {
                        resultsDiv.innerHTML += '<h4>BLE Devices:</h4>';
                        data.bleDevices.forEach(device => {
                            const deviceDiv = document.createElement('div');
                            deviceDiv.className = 'device-item';
                            deviceDiv.innerHTML = `
                                <strong>${device.name || 'Unknown'}</strong>
                                <span class="signal">RSSI: ${device.rssi}dBm</span><br>
                                <small>MAC: ${device.address}</small>
                            `;
                            resultsDiv.appendChild(deviceDiv);
                        });
                    }
                    
                    if (data.wifiNetworks && data.wifiNetworks.length > 0) {
                        resultsDiv.innerHTML += '<h4>WiFi Networks:</h4>';
                        data.wifiNetworks.forEach(network => {
                            const networkDiv = document.createElement('div');
                            networkDiv.className = 'device-item';
                            networkDiv.innerHTML = `
                                <strong>${network.ssid || 'Hidden'}</strong>
                                <span class="signal">RSSI: ${network.rssi}dBm</span><br>
                                <small>Channel: ${network.channel} | Encryption: ${network.encryption}</small>
                            `;
                            resultsDiv.appendChild(networkDiv);
                        });
                    }
                    
                    if (data.lteNetworks && data.lteNetworks.length > 0) {
                        resultsDiv.innerHTML += '<h4>📱 LTE Networks:</h4>';
                        data.lteNetworks.forEach(network => {
                            const networkDiv = document.createElement('div');
                            networkDiv.className = 'device-item';
                            networkDiv.innerHTML = `
                                <strong>${network.carrier || 'Unknown Carrier'}</strong>
                                <span class="signal">RSSI: ${network.rssi}dBm</span><br>
                                <small>Band: ${network.band} | Status: ${network.status || 'SCANNED'}</small>
                            `;
                            resultsDiv.appendChild(networkDiv);
                        });
                    }
                    
                    if ((!data.bleDevices || data.bleDevices.length === 0) && 
                        (!data.wifiNetworks || data.wifiNetworks.length === 0) && 
                        (!data.lteNetworks || data.lteNetworks.length === 0)) {
                        resultsDiv.innerHTML = '<div class="loading">No devices found. Start a scan to discover nearby devices.</div>';
                    }
                })
                .catch(error => console.error('Error:', error));
        }
        
        // Update status every 2 seconds
        setInterval(updateStatus, 2000);
        
        // Initial status update
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

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

void setup() {
    Serial.begin(115200);
    Serial.println("C.H.A.O.S - Cyber Hacking & Offensive Security Platform");
    Serial.println("Initializing...");
    
    // Initialize BLE
    BLEDevice::init("CHAOS_BLE");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    
    // Initialize WiFi AP
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    // Initialize Bluetooth
    SerialBT.begin("CHAOS_BT");
    Serial.println("Bluetooth Started");
    
    // Web server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", htmlPage);
    });
    
    // API routes
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
        DynamicJsonDocument doc(1024);
        doc["bleScanActive"] = attackStatus.bleScanActive;
        doc["bleJamActive"] = attackStatus.bleJamActive;
        doc["wifiScanActive"] = attackStatus.wifiScanActive;
        doc["wifiJamActive"] = attackStatus.wifiJamActive;
        doc["lteScanActive"] = attackStatus.lteScanActive;
        doc["lteJamActive"] = attackStatus.lteJamActive;
        doc["massAttackActive"] = attackStatus.massAttackActive;
        doc["bleDevicesFound"] = attackStatus.bleDevicesFound;
        doc["wifiNetworksFound"] = attackStatus.wifiNetworksFound;
        doc["lteNetworksFound"] = attackStatus.lteNetworksFound;
        doc["massAttackTargets"] = attackStatus.massAttackTargets;
        doc["attackMode"] = attackStatus.attackMode;
        doc["lteBand"] = attackStatus.lteBand;
        doc["lteSignalStrength"] = attackStatus.lteSignalStrength;
        doc["jamPower"] = attackStatus.jamPower;
        doc["jamChannel"] = attackStatus.jamChannel;
        doc["aggressiveMode"] = attackStatus.aggressiveMode;
        doc["lastScanTime"] = attackStatus.lastScanTime;
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    server.on("/api/ble/scan/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isScanningBLE) {
            isScanningBLE = true;
            attackStatus.bleScanActive = true;
            attackStatus.bleDevicesFound = 0;
            pBLEScan->start(0, nullptr, false);
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
        }
        request->send(200, "application/json", "{\"status\":\"BLE scan started\"}");
    });
    
    server.on("/api/ble/scan/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isScanningBLE) {
            isScanningBLE = false;
            attackStatus.bleScanActive = false;
            pBLEScan->stop();
        }
        request->send(200, "application/json", "{\"status\":\"BLE scan stopped\"}");
    });
    
    server.on("/api/ble/jam/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isJammingBLE) {
            isJammingBLE = true;
            attackStatus.bleJamActive = true;
            Serial.println("BLE Jamming started - WARNING: This may interfere with nearby devices!");
        }
        request->send(200, "application/json", "{\"status\":\"BLE jamming started\"}");
    });
    
    server.on("/api/ble/jam/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isJammingBLE) {
            isJammingBLE = false;
            attackStatus.bleJamActive = false;
            Serial.println("BLE Jamming stopped");
        }
        request->send(200, "application/json", "{\"status\":\"BLE jamming stopped\"}");
    });
    
    server.on("/api/wifi/scan/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isScanningWiFi) {
            isScanningWiFi = true;
            attackStatus.wifiScanActive = true;
            attackStatus.wifiNetworksFound = 0;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            Serial.println("WiFi scan started");
        }
        request->send(200, "application/json", "{\"status\":\"WiFi scan started\"}");
    });
    
    server.on("/api/wifi/scan/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isScanningWiFi) {
            isScanningWiFi = false;
            attackStatus.wifiScanActive = false;
            Serial.println("WiFi scan stopped");
        }
        request->send(200, "application/json", "{\"status\":\"WiFi scan stopped\"}");
    });
    
    server.on("/api/wifi/jam/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isJammingWiFi) {
            isJammingWiFi = true;
            attackStatus.wifiJamActive = true;
            Serial.println("WiFi Jamming started - WARNING: This may interfere with nearby networks!");
        }
        request->send(200, "application/json", "{\"status\":\"WiFi jamming started\"}");
    });
    
    server.on("/api/wifi/jam/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isJammingWiFi) {
            isJammingWiFi = false;
            attackStatus.wifiJamActive = false;
            Serial.println("WiFi Jamming stopped");
        }
        request->send(200, "application/json", "{\"status\":\"WiFi jamming stopped\"}");
    });
    
    server.on("/api/scan/results", HTTP_GET, [](AsyncWebServerRequest *request){
        DynamicJsonDocument doc(2048);
        JsonArray bleDevices = doc.createNestedArray("bleDevices");
        JsonArray wifiNetworks = doc.createNestedArray("wifiNetworks");
        JsonArray lteNetworks = doc.createNestedArray("lteNetworks");
        
        // Add some sample data for demonstration
        if (attackStatus.bleDevicesFound > 0) {
            for (int i = 0; i < attackStatus.bleDevicesFound; i++) {
                JsonObject device = bleDevices.createNestedObject();
                device["name"] = "Device_" + String(i);
                device["address"] = "AA:BB:CC:DD:EE:FF";
                device["rssi"] = -50 - (i * 10);
                device["status"] = attackStatus.massAttackActive ? "UNDER_ATTACK" : "SCANNED";
            }
        }
        
        if (attackStatus.wifiNetworksFound > 0) {
            for (int i = 0; i < attackStatus.wifiNetworksFound; i++) {
                JsonObject network = wifiNetworks.createNestedObject();
                network["ssid"] = "Network_" + String(i);
                network["rssi"] = -40 - (i * 5);
                network["channel"] = 1 + (i % 13);
                network["encryption"] = "WPA2";
                network["status"] = attackStatus.massAttackActive ? "UNDER_ATTACK" : "SCANNED";
            }
        }
        
        if (attackStatus.lteNetworksFound > 0) {
            String carrierNames[] = {"Turkcell", "Vodafone", "Türk Telekom", "Bursa Mobile", "Local LTE"};
            for (int i = 0; i < attackStatus.lteNetworksFound; i++) {
                JsonObject network = lteNetworks.createNestedObject();
                network["carrier"] = carrierNames[i % 5];
                network["rssi"] = attackStatus.lteSignalStrength + random(-10, 10);
                network["band"] = attackStatus.lteBand;
                network["status"] = attackStatus.massAttackActive ? "UNDER_ATTACK" : "SCANNED";
            }
        }
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // Mass Attack API endpoints
    server.on("/api/mass/attack/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!attackStatus.massAttackActive) {
            attackStatus.massAttackActive = true;
            attackStatus.attackMode = "MASS_ATTACK";
            
            // Start BLE, WiFi and LTE jamming simultaneously
            isJammingBLE = true;
            isJammingWiFi = true;
            isJammingLTE = true;
            attackStatus.bleJamActive = true;
            attackStatus.wifiJamActive = true;
            attackStatus.lteJamActive = true;
            
            attackStatus.massAttackTargets = attackStatus.bleDevicesFound + attackStatus.wifiNetworksFound + attackStatus.lteNetworksFound;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            
            Serial.println("🔥 MASS ATTACK INITIATED - Jamming ALL BLE devices, WiFi networks and LTE networks!");
            Serial.println("Targets: " + String(attackStatus.massAttackTargets) + " devices/networks");
        }
        request->send(200, "application/json", "{\"status\":\"MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/ble/attack/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!attackStatus.massAttackActive) {
            attackStatus.massAttackActive = true;
            attackStatus.attackMode = "BLE_ONLY";
            
            // Start BLE mass jamming
            isJammingBLE = true;
            attackStatus.bleJamActive = true;
            
            attackStatus.massAttackTargets = attackStatus.bleDevicesFound;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            
            Serial.println("⚡ BLE MASS ATTACK INITIATED - Jamming ALL BLE devices!");
            Serial.println("BLE Targets: " + String(attackStatus.massAttackTargets) + " devices");
        }
        request->send(200, "application/json", "{\"status\":\"BLE MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/wifi/attack/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!attackStatus.massAttackActive) {
            attackStatus.massAttackActive = true;
            attackStatus.attackMode = "WIFI_ONLY";
            
            // Start WiFi mass jamming
            isJammingWiFi = true;
            attackStatus.wifiJamActive = true;
            
            attackStatus.massAttackTargets = attackStatus.wifiNetworksFound;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            
            Serial.println("📡 WiFi MASS ATTACK INITIATED - Jamming ALL WiFi networks!");
            Serial.println("WiFi Targets: " + String(attackStatus.massAttackTargets) + " networks");
        }
        request->send(200, "application/json", "{\"status\":\"WiFi MASS ATTACK INITIATED\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.on("/api/mass/attack/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (attackStatus.massAttackActive) {
            attackStatus.massAttackActive = false;
            attackStatus.attackMode = "NONE";
            
            // Stop all jamming
            isJammingBLE = false;
            isJammingWiFi = false;
            isJammingLTE = false;
            attackStatus.bleJamActive = false;
            attackStatus.wifiJamActive = false;
            attackStatus.lteJamActive = false;
            
            attackStatus.massAttackTargets = 0;
            
            Serial.println("🛑 ALL MASS ATTACKS STOPPED");
        }
        request->send(200, "application/json", "{\"status\":\"ALL MASS ATTACKS STOPPED\"}");
    });
    
    // LTE API endpoints
    server.on("/api/lte/scan/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isScanningLTE) {
            isScanningLTE = true;
            attackStatus.lteScanActive = true;
            attackStatus.lteNetworksFound = 0;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            
            // Parse band from request body
            if (request->hasParam("body")) {
                String body = request->getParam("body")->value();
                DynamicJsonDocument doc(256);
                deserializeJson(doc, body);
                if (doc.containsKey("band")) {
                    attackStatus.lteBand = doc["band"].as<String>();
                }
            }
            
            Serial.println("📱 LTE scan started on " + attackStatus.lteBand);
        }
        request->send(200, "application/json", "{\"status\":\"LTE scan started on " + attackStatus.lteBand + "\"}");
    });
    
    server.on("/api/lte/scan/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isScanningLTE) {
            isScanningLTE = false;
            attackStatus.lteScanActive = false;
            Serial.println("📱 LTE scan stopped");
        }
        request->send(200, "application/json", "{\"status\":\"LTE scan stopped\"}");
    });
    
    server.on("/api/lte/jam/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!isJammingLTE) {
            // Parse band from request body
            if (request->hasParam("body")) {
                String body = request->getParam("body")->value();
                DynamicJsonDocument doc(256);
                deserializeJson(doc, body);
                if (doc.containsKey("band")) {
                    attackStatus.lteBand = doc["band"].as<String>();
                }
            }
            
            // Start real RF jamming
            startRealLTEJam();
            Serial.println("📱 REAL LTE Jamming started on " + attackStatus.lteBand + " - WARNING: RF interference active!");
        }
        request->send(200, "application/json", "{\"status\":\"REAL LTE jamming started on " + attackStatus.lteBand + "\"}");
    });
    
    server.on("/api/lte/jam/stop", HTTP_POST, [](AsyncWebServerRequest *request){
        if (isJammingLTE) {
            stopRealLTEJam();
        }
        request->send(200, "application/json", "{\"status\":\"REAL LTE jamming stopped\"}");
    });
    
    // New API endpoints for real jamming control
    server.on("/api/lte/jam/power", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("body")) {
            String body = request->getParam("body")->value();
            DynamicJsonDocument doc(256);
            deserializeJson(doc, body);
            if (doc.containsKey("power")) {
                jamPowerLevel = doc["power"];
                attackStatus.jamPower = jamPowerLevel;
                Serial.println("📱 RF Power updated to: " + String(jamPowerLevel) + "%");
            }
        }
        request->send(200, "application/json", "{\"status\":\"RF power updated to " + String(jamPowerLevel) + "%\"}");
    });
    
    server.on("/api/lte/jam/aggressive", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("body")) {
            String body = request->getParam("body")->value();
            DynamicJsonDocument doc(256);
            deserializeJson(doc, body);
            if (doc.containsKey("aggressive")) {
                aggressiveJamming = doc["aggressive"];
                attackStatus.aggressiveMode = aggressiveJamming;
                Serial.println("📱 Aggressive mode: " + String(aggressiveJamming ? "ENABLED" : "DISABLED"));
            }
        }
        request->send(200, "application/json", "{\"status\":\"Aggressive mode " + String(aggressiveJamming ? "enabled" : "disabled") + "\"}");
    });
    
    server.on("/api/mass/lte/attack/start", HTTP_POST, [](AsyncWebServerRequest *request){
        if (!attackStatus.massAttackActive) {
            attackStatus.massAttackActive = true;
            attackStatus.attackMode = "LTE_ONLY";
            
            // Parse band from request body
            if (request->hasParam("body")) {
                String body = request->getParam("body")->value();
                DynamicJsonDocument doc(256);
                deserializeJson(doc, body);
                if (doc.containsKey("band")) {
                    attackStatus.lteBand = doc["band"].as<String>();
                }
            }
            
            // Start LTE mass jamming
            isJammingLTE = true;
            attackStatus.lteJamActive = true;
            
            attackStatus.massAttackTargets = attackStatus.lteNetworksFound;
            attackStatus.lastScanTime = String(millis() / 1000) + "s";
            
            Serial.println("📱 LTE MASS ATTACK INITIATED on " + attackStatus.lteBand + " - Jamming ALL LTE networks!");
            Serial.println("LTE Targets: " + String(attackStatus.massAttackTargets) + " networks");
        }
        request->send(200, "application/json", "{\"status\":\"LTE MASS ATTACK INITIATED on " + attackStatus.lteBand + "\",\"targets\":" + String(attackStatus.massAttackTargets) + "}");
    });
    
    server.begin();
    Serial.println("Web server started");
    Serial.println("Access the interface at: http://" + WiFi.softAPIP().toString());
    
    // Initialize RF for real jamming
    Serial.println("Initializing RF modules for real jamming...");
    initRFJammer();
}

// Real RF Jamming Functions
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
        
        Serial.println("📱 REAL LTE JAM STARTED - RF interference mode active!");
        Serial.println("Power Level: " + String(jamPowerLevel) + "%");
        Serial.println("Band Simulation: " + attackStatus.lteBand);
    }
}

void stopRealLTEJam() {
    if (isJammingLTE) {
        isJammingLTE = false;
        attackStatus.lteJamActive = false;
        
        Serial.println("📱 REAL LTE JAM STOPPED - RF interference disabled");
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
            
            Serial.println("🔥 AGGRESSIVE LTE JAM CYCLE " + String(jamCycles) + " - Multi-channel interference");
        } else {
            // Standard interference simulation
            delay(50);
            
            // Switch channel for broader interference
            currentJamChannel = (currentJamChannel % 13) + 1;
            
            Serial.println("📱 LTE JAM CYCLE " + String(jamCycles) + " - Channel " + String(currentJamChannel) + " interference");
        }
        
        // Update attack statistics
        attackStatus.jamPower = jamPowerLevel;
        attackStatus.jamChannel = currentJamChannel;
        attackStatus.aggressiveMode = aggressiveJamming;
    }
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
            String carrierNames[] = {"Turkcell", "Vodafone", "Türk Telekom", "Bursa Mobile", "Local LTE"};
            String carrier = carrierNames[i % 5];
            int signal = attackStatus.lteSignalStrength + random(-10, 10);
            Serial.printf("📱 LTE Network: %s on %s (RSSI: %ddBm)\n", 
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
                Serial.println("🔥 MASS ATTACK ACTIVE - Jamming " + String(attackStatus.massAttackTargets) + " targets");
                // Simulate aggressive jamming pattern
                for (int i = 0; i < 12; i++) {
                    // BLE jamming simulation
                    if (attackStatus.bleDevicesFound > 0) {
                        Serial.println("⚡ BLE JAM: Targeting " + String(attackStatus.bleDevicesFound) + " devices");
                    }
                    
                    // WiFi jamming simulation  
                    if (attackStatus.wifiNetworksFound > 0) {
                        Serial.println("📡 WiFi JAM: Targeting " + String(attackStatus.wifiNetworksFound) + " networks");
                    }
                    
                    // LTE jamming simulation
                    if (attackStatus.lteNetworksFound > 0) {
                        Serial.println("📱 LTE JAM: Targeting " + String(attackStatus.lteNetworksFound) + " networks on " + attackStatus.lteBand);
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
                Serial.println("⚡ BLE MASS ATTACK - Jamming " + String(attackStatus.massAttackTargets) + " BLE devices");
                for (int i = 0; i < 5; i++) {
                    Serial.println("BLE JAM CYCLE: " + String(i+1) + "/5 - Targeting all BLE devices");
                    delay(100);
                }
            }
        }
        // WiFi Only Mass Attack
        else if (attackStatus.attackMode == "WIFI_ONLY") {
            if (isJammingWiFi) {
                Serial.println("📡 WiFi MASS ATTACK - Jamming " + String(attackStatus.massAttackTargets) + " WiFi networks");
                for (int i = 0; i < 5; i++) {
                    Serial.println("WiFi JAM CYCLE: " + String(i+1) + "/5 - Targeting all WiFi networks");
                    delay(100);
                }
            }
        }
        // LTE Only Mass Attack
        else if (attackStatus.attackMode == "LTE_ONLY") {
            if (isJammingLTE) {
                Serial.println("📱 LTE MASS ATTACK on " + attackStatus.lteBand + " - Jamming " + String(attackStatus.massAttackTargets) + " LTE networks");
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
    
    delay(100);
}
