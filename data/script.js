let statusUpdateInterval;

function addLog(message, type = 'info') {
    const logs = document.getElementById('logs');
    const logEntry = document.createElement('div');
    logEntry.className = `log-entry ${type}`;
    logEntry.textContent = new Date().toLocaleTimeString() + ' - ' + message;
    logs.appendChild(logEntry);
    logs.scrollTop = logs.scrollHeight;
}

function updateStatus() {
    fetch('/api/status')
        .then(response => response.json())
        .then(data => {
            // Update BLE status
            document.getElementById('bleScanStatus').textContent = data.bleScanActive ? 'ACTIVE' : 'INACTIVE';
            document.getElementById('bleDevicesFound').textContent = data.bleDevicesFound;
            document.getElementById('bleJamStatusText').textContent = data.bleJamActive ? 'ACTIVE' : 'INACTIVE';
            
            const bleStatusIndicator = document.getElementById('bleStatus');
            const bleJamStatusIndicator = document.getElementById('bleJamStatus');
            
            if (data.bleScanActive) {
                bleStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                bleStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            if (data.bleJamActive) {
                bleJamStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                bleJamStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            // Update WiFi status
            document.getElementById('wifiScanStatus').textContent = data.wifiScanActive ? 'ACTIVE' : 'INACTIVE';
            document.getElementById('wifiNetworksFound').textContent = data.wifiNetworksFound;
            document.getElementById('wifiJamStatusText').textContent = data.wifiJamActive ? 'ACTIVE' : 'INACTIVE';
            
            const wifiStatusIndicator = document.getElementById('wifiStatus');
            const wifiJamStatusIndicator = document.getElementById('wifiJamStatus');
            
            if (data.wifiScanActive) {
                wifiStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                wifiStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            if (data.wifiJamActive) {
                wifiJamStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                wifiJamStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            // Update LTE status
            document.getElementById('lteScanStatus').textContent = data.lteScanActive ? 'ACTIVE' : 'INACTIVE';
            document.getElementById('lteNetworksFound').textContent = data.lteNetworksFound;
            document.getElementById('lteJamStatusText').textContent = data.lteJamActive ? 'ACTIVE' : 'INACTIVE';
            document.getElementById('lteBand').textContent = data.lteBand;
            document.getElementById('ltePower').textContent = data.jamPower + '%';
            
            const lteStatusIndicator = document.getElementById('lteStatus');
            const lteJamStatusIndicator = document.getElementById('lteJamStatus');
            
            if (data.lteScanActive) {
                lteStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                lteStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            if (data.lteJamActive) {
                lteJamStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                lteJamStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            // Update Mass Attack status
            document.getElementById('massAttackStatusText').textContent = data.massAttackActive ? 'ACTIVE' : 'INACTIVE';
            document.getElementById('massAttackTargets').textContent = data.massAttackTargets;
            document.getElementById('attackMode').textContent = data.attackMode;
            
            const massAttackStatusIndicator = document.getElementById('massAttackStatus');
            
            if (data.massAttackActive) {
                massAttackStatusIndicator.className = 'status-indicator status-active pulse';
            } else {
                massAttackStatusIndicator.className = 'status-indicator status-inactive';
            }
            
            // Update system status
            document.getElementById('systemStatus').textContent = 'ONLINE';
        })
        .catch(error => {
            console.error('Status update error:', error);
            addLog('Status update failed', 'error');
        });
}

// BLE Functions
function startBLEScan() {
    fetch('/api/ble/scan/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('BLE scan started: ' + data.status);
        });
}

function stopBLEScan() {
    fetch('/api/ble/scan/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('BLE scan stopped: ' + data.status);
        });
}

function startBLEJam() {
    fetch('/api/ble/jam/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('BLE jamming started: ' + data.status);
        });
}

function stopBLEJam() {
    fetch('/api/ble/jam/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('BLE jamming stopped: ' + data.status);
        });
}

// WiFi Functions
function startWiFiScan() {
    fetch('/api/wifi/scan/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('WiFi scan started: ' + data.status);
        });
}

function stopWiFiScan() {
    fetch('/api/wifi/scan/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('WiFi scan stopped: ' + data.status);
        });
}

function startWiFiJam() {
    fetch('/api/wifi/jam/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('WiFi jamming started: ' + data.status);
        });
}

function stopWiFiJam() {
    fetch('/api/wifi/jam/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('WiFi jamming stopped: ' + data.status);
        });
}

// LTE Functions
function startLTEScan() {
    fetch('/api/lte/scan/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('LTE scan started: ' + data.status);
        });
}

function stopLTEScan() {
    fetch('/api/lte/scan/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('LTE scan stopped: ' + data.status);
        });
}

function startLTEJam() {
    fetch('/api/lte/jam/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('LTE jamming started: ' + data.status);
        });
}

function stopLTEJam() {
    fetch('/api/lte/jam/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('LTE jamming stopped: ' + data.status);
        });
}

function updateJamPower(value) {
    document.getElementById('powerValue').textContent = value + '%';
    fetch('/api/lte/jam/power', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({power: parseInt(value)})
    })
    .then(response => response.json())
    .then(data => {
        addLog('RF power updated: ' + data.status);
    });
}

function updateAggressiveMode(checked) {
    fetch('/api/lte/jam/aggressive', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({aggressive: checked})
    })
    .then(response => response.json())
    .then(data => {
        addLog('Aggressive mode: ' + data.status);
    });
}

// Mass Attack Functions
function startMassAttack() {
    fetch('/api/mass/attack/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('MASS ATTACK INITIATED: ' + data.status, 'warning');
        });
}

function startMassBLEAttack() {
    fetch('/api/mass/ble/attack/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('BLE MASS ATTACK INITIATED: ' + data.status, 'warning');
        });
}

function startMassWiFiAttack() {
    fetch('/api/mass/wifi/attack/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('WiFi MASS ATTACK INITIATED: ' + data.status, 'warning');
        });
}

function startMassLTEAttack() {
    fetch('/api/mass/lte/attack/start', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('LTE MASS ATTACK INITIATED: ' + data.status, 'warning');
        });
}

function stopAllMassAttacks() {
    fetch('/api/mass/attack/stop', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            addLog('ALL MASS ATTACKS STOPPED: ' + data.status, 'warning');
        });
}

// Initialize
document.addEventListener('DOMContentLoaded', function() {
    addLog('C.H.A.O.S interface loaded successfully');
    statusUpdateInterval = setInterval(updateStatus, 1000);
    updateStatus();
});
