#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define AP_SSID "CHAOS_AP"
#define AP_PASSWORD "chaos123456"
#define AP_CHANNEL 1
#define AP_MAX_CONNECTIONS 4

// Web Server Configuration
#define WEB_SERVER_PORT 80
#define WEB_SOCKET_PORT 81

// BLE Configuration
#define BLE_DEVICE_NAME "CHAOS_BLE"
#define BLE_SCAN_INTERVAL 100
#define BLE_SCAN_WINDOW 99
#define BLE_SCAN_DURATION 10

// WiFi Scan Configuration
#define WIFI_SCAN_INTERVAL 5000
#define WIFI_MAX_NETWORKS 20

// System Configuration
#define SERIAL_BAUD_RATE 115200
#define STATUS_UPDATE_INTERVAL 2000

// Pin Definitions (if needed for future hardware extensions)
#define LED_PIN 2
#define BUTTON_PIN 0

// Debug Configuration
#define DEBUG_ENABLED true
#define VERBOSE_LOGGING true

// Security Configuration
#define MAX_CONCURRENT_SCANS 2
#define SCAN_TIMEOUT_MS 30000

#endif // CONFIG_H
