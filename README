# ESP8266 SmartDevices Control System

A modern web-based control system for ESP8266 microcontrollers to control lights and monitor temperature with a beautiful, animated user interface.

## 📋 Overview

This project implements a WiFi-enabled smart home control system using an ESP8266 microcontroller. It creates a web server that serves a modern, responsive web interface for controlling connected devices and monitoring sensors.

## ✨ Features

- **LED Control**: Turn lights on/off remotely via web interface
- **Temperature Monitoring**: Real-time temperature display (ready for DS18B20 sensor integration)
- **Modern Web UI**: Beautiful dark-themed interface with animations
  - Glassmorphism design
  - Smooth hover and click animations
  - Responsive layout for mobile and desktop
  - Status indicators with pulsing effects
- **WiFi Connectivity**: Accessible from any device on your network

## 🛠️ Hardware Requirements

- **ESP8266 NodeMCU** or similar ESP8266 board
- **Built-in LED** (GPIO2/D4 on NodeMCU)
- **(Optional) DS18B20 Temperature Sensor** - Connect to GPIO5 (D1) for real temperature readings

## 📦 Software Requirements

### Arduino IDE Libraries:
- `ESP8266WiFi` - WiFi connectivity (included with ESP8266 board package)
- `ESP8266WebServer` - Web server functionality (included with ESP8266 board package)

### Optional (for real temperature sensor):
- `OneWire` - For DS18B20 communication
- `DallasTemperature` - For DS18B20 temperature sensor

## 🔧 Configuration

### WiFi Settings
Edit these lines in `esp8266.ino`:
```cpp
const char* ssid = "test";        // Your WiFi network name
const char* password = "testpw";  // Your WiFi password
```

### LED Pin
```cpp
const int LED_PIN = 2;  // GPIO2 (D4 on NodeMCU)
```

## 📁 File Structure

### `esp8266.ino`
Main Arduino sketch containing:
- WiFi connection setup
- Web server configuration
- HTML/CSS/JavaScript interface (embedded in PROGMEM)
- API endpoints:
  - `/` - Main web interface
  - `/led?state=on|off` - LED control endpoint
  - `/temperature` - Temperature reading endpoint

### `test.html`
Standalone HTML file for testing and preview. Contains the same interface as embedded in the `.ino` file.

## 🚀 Installation & Usage

1. **Install ESP8266 Board Support**:
   - Open Arduino IDE
   - Go to File → Preferences
   - Add to "Additional Board Manager URLs": 
     `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Go to Tools → Board → Board Manager
   - Search for "esp8266" and install

2. **Configure WiFi**:
   - Edit `ssid` and `password` in the sketch

3. **Upload**:
   - Select board: Tools → Board → ESP8266 Boards → NodeMCU 1.0
   - Select port: Tools → Port → (your ESP8266 port)
   - Click Upload

4. **Find IP Address**:
   - Open Serial Monitor (115200 baud)
   - Wait for "Connected!" message
   - Note the displayed IP address (e.g., `192.168.1.123`)

5. **Access Interface**:
   - Open web browser
   - Navigate to the IP address shown in Serial Monitor
   - Enjoy the modern control interface!

## 🎨 Web Interface Features

The embedded web interface includes:
- **Gradient background** with animated floating orbs
- **Glassmorphic cards** with blur effects
- **Interactive buttons** with ripple animations
- **Status indicator** showing light state with pulsing dot
- **Temperature display** with gradient text and subtle pulse animation
- **Responsive design** adapting to all screen sizes

## 🔌 API Endpoints

### `GET /`
Returns the main HTML interface

### `GET /led?state=on`
Turns the LED on
- Returns: `"on"` (text/plain)

### `GET /led?state=off`
Turns the LED off
- Returns: `"off"` (text/plain)

### `GET /temperature`
Returns current temperature reading
- Returns: `"24.7"` (text/plain, dummy value)
- Ready for DS18B20 sensor integration

## 🌡️ Adding a Real Temperature Sensor (DS18B20)

To use a real DS18B20 temperature sensor:

1. **Install libraries**:
   - OneWire
   - DallasTemperature

2. **Connect hardware**:
   - DS18B20 Data → GPIO5 (D1)
   - DS18B20 VCC → 3.3V
   - DS18B20 GND → GND
   - Add 4.7kΩ pull-up resistor between Data and VCC

3. **Update code**:
   - Uncomment DS18B20 code in the sketch
   - Replace dummy `getTemperature()` function with actual sensor reading

## 🐛 Troubleshooting

**Can't connect to WiFi:**
- Check SSID and password
- Ensure 2.4GHz WiFi (ESP8266 doesn't support 5GHz)
- Check Serial Monitor for connection messages

**Can't access web interface:**
- Verify IP address in Serial Monitor
- Ensure device is on same network as ESP8266
- Try pinging the IP address

**LED not working:**
- Note: NodeMCU LED is active LOW (lights up when pin is LOW)
- Check LED_PIN matches your board

## 📝 Technical Details

- **Microcontroller**: ESP8266
- **Operating Voltage**: 3.3V
- **WiFi**: 802.11 b/g/n (2.4GHz)
- **Web Server**: ESP8266WebServer library
- **Temperature Update Interval**: 2 seconds
- **Serial Baud Rate**: 115200

## 🎓 Educational Notes

This project demonstrates:
- IoT device programming
- Web server implementation on microcontrollers
- RESTful API design
- Modern web interface development
- Responsive design principles
- CSS animations and transitions
- Asynchronous JavaScript (async/await)

## 📄 License

Educational project for Berufsschule ITT course.

## 👤 Author

Created for 2nd year ITT coursework - Berufsschule