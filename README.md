# C.H.A.O.S - Cyber Hacking & Offensive Security Platform

## ⚠️ WARNING ⚠️
**This project is for educational and testing purposes only. Use only on your own devices and networks. Unauthorized use on others' devices or networks is illegal and unethical.**

## Overview
C.H.A.O.S (Cyber Hacking & Offensive Security) is an ESP32-S based platform designed for testing and educational purposes. It provides a web-based interface for scanning and analyzing BLE and WiFi networks in your environment.

## Features
- **BLE Device Scanning**: Discover nearby Bluetooth Low Energy devices
- **WiFi Network Scanning**: Analyze available WiFi networks
- **Web-based Interface**: Modern, hacker-themed UI for easy control
- **Real-time Status Updates**: Live monitoring of scan results and system status
- **Educational Focus**: Learn about wireless security and network analysis

## ✨ Features

### 🔍 **Multi-Protocol Scanning**
- **BLE Device Discovery** - Scan and analyze nearby Bluetooth Low Energy devices
- **WiFi Network Analysis** - Comprehensive WiFi network scanning and analysis
- **LTE Network Detection** - Simulated LTE network discovery and analysis

### ⚡ **Advanced Jamming Capabilities**
- **Real RF Interference** - Actual RF jamming using ESP32's WiFi radio
- **Multi-Channel Attacks** - Simultaneous jamming across multiple channels
- **Power Control** - Adjustable RF power levels (0-82%)
- **Aggressive Mode** - High-intensity interference patterns

### 🎯 **Mass Attack Operations**
- **🔥 MASS ATTACK ALL** - Simultaneous jamming of BLE, WiFi, and LTE
- **⚡ BLE MASS JAM** - Targeted BLE device jamming
- **📡 WiFi MASS JAM** - Comprehensive WiFi network jamming
- **📱 LTE MASS JAM** - Advanced LTE interference simulation

### 🌐 **Modern Web Interface**
- **Hacker-themed UI** - Dark, professional interface with green accents
- **Real-time Monitoring** - Live status updates and attack progress
- **Responsive Design** - Works on desktop and mobile devices
- **RESTful API** - Complete API for automation and integration

## 🔧 Hardware Requirements

- **ESP32-S3** development board
- **USB-C cable** for programming and power
- **Computer** with PlatformIO installed
- **WiFi-enabled device** for web interface access

## 📋 Software Requirements

- **PlatformIO IDE** or **PlatformIO Core**
- **Arduino Framework**
- **Required Libraries** (automatically installed via platformio.ini):
  - WebServer
  - WiFi
  - BluetoothSerial
  - ESP32-BLE-Arduino
  - ArduinoJson
  - AsyncTCP
  - ESPAsyncWebServer

## 🚀 Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/z3x0fly/chaos.git
cd chaos
```

### 2. Open in PlatformIO
- Launch **PlatformIO IDE** or **VS Code with PlatformIO extension**
- Open the `chaos` project folder

### 3. Build and Upload
```bash
# Using PlatformIO CLI
pio run -t upload

# Or using PlatformIO IDE
# Click "Build" then "Upload"
```

### 4. Access the Interface
1. After upload, ESP32 creates WiFi access point: **CHAOS_AP**
2. Connect using password: **chaos123456**
3. Open browser and navigate to: **192.168.4.1**

## 🎮 Usage Guide

### 📡 **Basic Scanning**
1. **Start BLE Scan** - Discover nearby Bluetooth devices
2. **Start WiFi Scan** - Analyze available WiFi networks
3. **Start LTE Scan** - Simulate LTE network discovery

### ⚡ **Jamming Operations**
1. **Individual Jamming** - Target specific protocols
2. **Power Control** - Adjust RF power level (0-82%)
3. **Aggressive Mode** - Enable high-intensity interference
4. **Mass Attacks** - Simultaneous multi-protocol jamming

### 🔥 **Mass Attack Modes**
- **MASS ATTACK ALL** - Jams all discovered targets simultaneously
- **Protocol-Specific** - Target only BLE, WiFi, or LTE networks
- **Real-time Control** - Start/stop operations instantly

## 🌐 API Reference

### **Status Endpoints**
```
GET /api/status - Get current system status
```

### **BLE Operations**
```
POST /api/ble/scan/start    - Start BLE scanning
POST /api/ble/scan/stop     - Stop BLE scanning
POST /api/ble/jam/start     - Start BLE jamming
POST /api/ble/jam/stop      - Stop BLE jamming
```

### **WiFi Operations**
```
POST /api/wifi/scan/start   - Start WiFi scanning
POST /api/wifi/scan/stop    - Stop WiFi scanning
POST /api/wifi/jam/start    - Start WiFi jamming
POST /api/wifi/jam/stop     - Stop WiFi jamming
```

### **LTE Operations**
```
POST /api/lte/scan/start    - Start LTE scanning
POST /api/lte/scan/stop     - Stop LTE scanning
POST /api/lte/jam/start     - Start LTE jamming
POST /api/lte/jam/stop      - Stop LTE jamming
POST /api/lte/jam/power     - Set RF power level
POST /api/lte/jam/aggressive - Toggle aggressive mode
```

### **Mass Attack Operations**
```
POST /api/mass/attack/start     - Start mass attack on all targets
POST /api/mass/ble/attack/start - Start BLE mass attack
POST /api/mass/wifi/attack/start - Start WiFi mass attack
POST /api/mass/lte/attack/start  - Start LTE mass attack
POST /api/mass/attack/stop      - Stop all attacks
```

### **Results**
```
GET /api/scan/results - Get scan results (BLE, WiFi, LTE)
```

## 🔬 Technical Specifications

### **RF Capabilities**
- **Frequency Range**: 2.4GHz (WiFi/BLE bands)
- **Power Output**: 0-82% (ESP32 maximum)
- **Channel Support**: 1-13 (WiFi channels)
- **Interference Patterns**: Configurable burst timing

### **Protocol Support**
- **Bluetooth Low Energy (BLE)**: Device discovery and analysis
- **WiFi (802.11)**: Network scanning and jamming
- **LTE Simulation**: Network discovery and interference simulation

### **Attack Modes**
- **Standard Mode**: Single-channel targeted jamming
- **Aggressive Mode**: Multi-channel rapid interference
- **Mass Attack**: Simultaneous multi-protocol jamming

## 📁 Project Structure

```
chaos/
├── platformio.ini          # PlatformIO configuration
├── src/
│   └── main.cpp            # Main application code
├── include/
│   └── config.h            # Configuration definitions
├── lib/
│   ├── chaos_core/         # Core system library
│   └── chaos_web/          # Web server library
├── data/
│   └── index.html          # Web interface (alternative)
├── README.md               # This file
└── .gitignore             # Git ignore rules
```

## ⚠️ Legal Disclaimer

This software is provided for **educational purposes only**. The authors and contributors are not responsible for any misuse of this software. Users are solely responsible for ensuring they comply with all applicable laws and regulations in their jurisdiction.

### **Important Notes:**
- **Use only on your own devices and networks**
- **Unauthorized use may be illegal in your jurisdiction**
- **May interfere with emergency communications**
- **RF emissions are subject to local regulations**

## 🤝 Contributing

This is an educational project. Contributions are welcome for:
- **Bug fixes and improvements**
- **Additional protocol support**
- **Enhanced security features**
- **Documentation improvements**

### **Development Setup:**
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the **Educational License**. This means:
- ✅ **Free for educational use**
- ✅ **Free for personal testing**
- ❌ **Commercial use requires permission**
- ❌ **Redistribution requires attribution**

## 🔗 Links

- **Repository**: [https://github.com/z3x0fly/chaos](https://github.com/z3x0fly/chaos)
- **PlatformIO**: [https://platformio.org/](https://platformio.org/)
- **ESP32-S3 Documentation**: [https://docs.espressif.com/projects/esp-idf/](https://docs.espressif.com/projects/esp-idf/)

## 🆘 Support

For questions and support:
- **Create an issue** on GitHub
- **Check the documentation** above
- **Review the code comments** for technical details

## 🏆 Acknowledgments

- **ESP32-S3** by Espressif Systems
- **PlatformIO** community
- **Arduino** framework developers
- **Open source** contributors

---

**Remember: With great power comes great responsibility. Use C.H.A.O.S ethically and legally!** 🛡️

---

*Made with ❤️ for the cybersecurity education community*