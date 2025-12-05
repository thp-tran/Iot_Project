# IoT Environmental Monitoring System

A comprehensive IoT solution for real-time environmental monitoring using ESP32 (YOLO UNO) with temperature and humidity sensors, TinyML anomaly detection, and web-based dashboard.

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Project Structure](#-project-structure)
- [Installation](#-installation)
  - [1. ESP32 Firmware Setup](#1-esp32-firmware-setup)
  - [2. Python-ML-Environment Setup](#2-python-ml-environment-setup)
  - [3. MQTT Gateway Setup](#3-mqtt-gateway-setup)
- [Configuration](#-configuration)
- [Usage](#-usage)
- [TinyML Model](#-tinyml-model)
- [MQTT Gateway](#-mqtt-gateway)
- [Web Dashboard](#-web-dashboard)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)
- [Credits](#-credits)

## 🔍 Overview

This project implements an IoT environmental monitoring system that:
- Monitors temperature and humidity using DHT20 sensors
- Performs real-time anomaly detection using TensorFlow Lite (TinyML)
- Sends telemetry data via MQTT to CoreIOT platform
- Provides a web-based dashboard for real-time visualization
- Supports both local and cloud MQTT broker configurations
- Features LED indicators controlled via web interface

## ✨ Features

### ESP32 Firmware
- **Real-time Sensor Monitoring**: DHT20 temperature and humidity sensor integration
- **TinyML Anomaly Detection**: On-device machine learning for anomaly detection
- **Multi-task FreeRTOS**: Efficient task management for concurrent operations
- **Web Server**: Built-in web interface for configuration and monitoring
- **MQTT Communication**: Support for both CoreIOT cloud and local MQTT brokers
- **OTA Updates**: Over-the-air firmware updates via ElegantOTA
- **LED Control**: NeoPixel and standard LED indicators
- **Wi-Fi Configuration**: Web-based Wi-Fi setup interface

### MQTT Gateway
- **Local MQTT Broker**: Aedes-based MQTT broker for local communication
- **Gateway Service**: Bridges local devices with CoreIOT cloud platform
- **Express API**: REST API for device management

### TinyML Model
- **Binary Classification**: Normal vs. anomaly detection
- **TensorFlow Lite**: Optimized for ESP32 deployment
- **Python Training Scripts**: Complete ML pipeline included

### Web Dashboard
- **Real-time Gauges**: Visual temperature and humidity displays
- **Device Control**: Toggle LEDs remotely
- **Responsive Design**: Mobile-friendly Vietnamese interface
- **JustGage Integration**: Beautiful gauge visualizations

## 🔧 Hardware Requirements

- **Microcontroller**: ESP32 (YOLO UNO board)
- **Sensor**: DHT20 temperature and humidity sensor
- **LEDs**: 
  - Standard LED (GPIO 48)
  - NeoPixel addressable LEDs
- **Power Supply**: USB or appropriate power source for ESP32

## 💻 Software Requirements

### ESP32 Development
- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- Platform: Espressif32
- Framework: Arduino

### Python Environment (for ML model training)
- Python 3.8.5
- TensorFlow 2.13.0
- scikit-learn 1.3.2
- pandas 2.0.3
- numpy 1.24.3
- See `python/requirements.txt` for complete list

### MQTT Gateway
- Node.js 18.x or higher
- npm or yarn

## 📁 Project Structure

```
Iot_Project/
├── src/                      # ESP32 firmware source files
│   ├── main.cpp             # Main application entry point
│   ├── coreiot.cpp          # CoreIOT MQTT implementation
│   ├── dht20_sensor.cpp     # DHT20 sensor driver
│   ├── tinyml.cpp           # TinyML inference
│   ├── mainserver.cpp       # Web server implementation
│   ├── led_blinky.cpp       # LED control
│   ├── neo_blinky.cpp       # NeoPixel control
│   └── temp_humi_monitor.cpp # Sensor monitoring
│
├── include/                  # Header files
│   ├── global.h             # Global definitions
│   ├── dht_anomaly_model.h  # TinyML model data
│   └── *.h                  # Component headers
│
├── data/                     # Web interface files (SPIFFS/LittleFS)
│   ├── index.html           # Dashboard HTML
│   ├── script.js            # Dashboard JavaScript
│   └── styles.css           # Dashboard styles
│
├── python/                   # Machine learning scripts
│   ├── TFL_For_MCU.py       # Model training & conversion
│   ├── generate_dataset_normal.py
│   ├── plot_decision_region.py
│   ├── plot_metrics.py
│   ├── test_model_single.py
│   └── requirements.txt     # Python dependencies
│
├── mqtt/                     # MQTT Gateway service
│   ├── src/
│   │   ├── index.js         # Gateway main
│   │   └── gateway/         # Gateway logic
│   ├── broker/
│   │   └── broker.js        # Local MQTT broker
│   ├── package.json
│   └── .env.example         # Environment template
│
├── lib/                      # External libraries
│   ├── ElegantOTA-master/   # OTA updates
│   ├── ArduinoJson/         # JSON parsing
│   └── ArduinoHttpClient/   # HTTP client
│
├── boards/
│   └── yolo_uno.json        # Custom board definition
│
├── platformio.ini           # PlatformIO configuration
└── README.md                # This file
```

## 🚀 Installation

### 1. ESP32 Firmware Setup
> **Note**: Make sure you have **PlatformIO** installed before proceeding.  
> If not, install it using: `pip install platformio` or via the PlatformIO IDE extension.

```bash
# Clone the repository
git clone https://github.com/thp-tran/Iot_Project.git
cd Iot_Project
```

### 2. Python ML Environment Setup
> **Note**: You can safely **skip this entire section** if you do not plan to **retrain the TinyML model**.  
> A pre-built model (`include/dht_anomaly_model.h`) is already included and ready to use.

```bash
cd python

# Create virtual environment
python -m venv iot_env
source iot_env/bin/activate  # On Windows: iot_env\Scripts\activate

# Install dependencies
pip install -r requirements.txt
```

### 3. MQTT Gateway Setup

```bash
cd mqtt

# Install dependencies
npm install

# Copy environment template
cp .env.example .env

# Edit .env with your configuration
nano .env
```

## ⚙️ Configuration

### ESP32 Configuration

Edit `src/coreiot.cpp`:

```cpp
const char *coreIOT_Server = "app.coreiot.io";      // CoreIOT Server
const char *coreIOT_Token = "YOUR_DEVICE_TOKEN";    // Device Access Token
IPAddress ip(192, 168, 102, 195);                   // Local MQTT broker IP
const int mqttPort = 1883;
bool isLocalServer = false;  // Set true for local MQTT broker
```

### MQTT Gateway Configuration

Edit `mqtt/.env`:

```env
TOKEN=YOUR_COREIOT_TOKEN
HOST=192.168.102.195           # Local broker host
COREIOT_HOST=app.coreiot.io    # Cloud broker host
PORT=1883
DEVICE_NAME=IoT_Gateway
```

## 📖 Usage

### Normal Operation

1. **Monitor Dashboard**: 
   - Find ESP32 IP address from serial monitor
   - The device will create a Wi-Fi access point named "ESP32-YOUR NETWORK HERE!!!" and use password "12345678"
   - Open browser to `http://192.168.4.1`
   - View real-time temperature and humidity

2. **Control LEDs**:
   - Use web dashboard toggles
   - LEDs can also be controlled via MQTT

3. **View Telemetry**:
   - MQTT topics:
     - Telemetry: `v1/devices/me/telemetry`
     - Attributes: `v1/devices/me/attributes`
     - RPC: `v1/devices/me/rpc/request/+`

### Running MQTT Services

1. **Power on ESP32**: Connect to Wi-Fi access point named "ESP32-YOUR NETWORK HERE!!!"
2. **Connect to AP**: Use password "12345678"
3. **Configure Wi-Fi**: 
   - Open browser to `http://192.168.4.1`
   - Click on Settings button UI
   - Enter your Wi-Fi credentials
   - Submit configuration
5. **Device Reboots**: ESP32 connects to your Wi-Fi network

```bash
# Start local MQTT broker
cd mqtt
npm run broker

# Start gateway service (in another terminal)
npm run dev
```

## 🤖 TinyML Model
> **Note**: This section is only required if you want to **retrain or modify the machine learning model**.  
> For normal usage of the project, **no action is needed here**.

### Training the Model

The project includes a binary classification model for detecting environmental anomalies:

```bash
cd python

# Generate synthetic dataset (if needed)
python generate_dataset_normal.py

# Train and convert model
python TFL_For_MCU.py
```

This will:
1. Train a neural network on `src/dataset.csv`
2. Convert to TensorFlow Lite format
3. Generate `include/dht_anomaly_model.h` for ESP32

### Model Architecture

- **Input**: 2 features (temperature, humidity)
- **Hidden Layer**: 10 neurons, ReLU activation
- **Output**: 1 neuron, sigmoid activation (binary classification)
- **Training**: 500 epochs, Adam optimizer

### Visualizing Model Performance

```bash
# Plot decision regions
python plot_decision_region.py

# Plot training metrics
python plot_metrics.py
```

## 🌐 MQTT Gateway

The gateway bridges local devices with the CoreIOT cloud platform:

- **Local Broker**: Runs Aedes MQTT broker for local communication
- **Cloud Bridge**: Forwards messages to/from CoreIOT
- **Auto-failover**: ESP32 automatically switches between local and cloud brokers

### MQTT Topics

#### Local Broker
- Telemetry: `esp32/telemetry`
- Attributes: `esp32/attributes`
- RPC: `esp32/devices/me/rpc/request/`

#### CoreIOT Cloud
- Telemetry: `v1/devices/me/telemetry`
- Attributes: `v1/devices/me/attributes`
- RPC: `v1/devices/me/rpc/request/+`

## 📊 Web Dashboard

The dashboard (`http://192.168.4.1`) provides:

- **Home Page**: Real-time gauges for temperature and humidity
- **Device Control**: Toggle LED states
- **Information**: System information
- **Settings**: Configuration options

Features:
- Responsive design
- Vietnamese language interface
- JustGage library for beautiful visualizations
- Font Awesome icons
- Real-time WebSocket updates

## 🔧 Troubleshooting

### ESP32 Won't Connect to Wi-Fi
- Verify Wi-Fi credentials in configuration portal
- Check Wi-Fi signal strength
- Reset device and reconfigure

### MQTT Connection Fails
- Verify CoreIOT token in `coreiot.cpp`
- Check network connectivity
- Ensure firewall allows port 1883
- Try local broker fallback

### Sensor Not Reading
- Check DHT20 I2C connections
- Verify sensor power supply

### Web Dashboard Not Loading
- Ensure filesystem was uploaded (`pio run --target uploadfs`)
- Check ESP32 IP address
- Verify all files in `data/` folder

### OTA Update Fails
- Ensure stable Wi-Fi connection
- Check firmware size (must fit in flash)
- Try updating via USB instead

### TinyML Model Not Working
- Verify `dht_anomaly_model.h` is in `include/` folder
- Check tensor arena size in `tinyml.cpp`
- Retrain model if needed

## 📄 License

This project is licensed under the ISC License.

## 🙏 Credits

### Hardware Platform
- **ESP32**: Espressif Systems
- **YOLO UNO**: Custom board based on ESP32

### Software Libraries
- **PlatformIO**: Development platform
- **TensorFlow Lite**: Machine learning inference
- **ElegantOTA**: OTA update library
- **ArduinoJson**: JSON library
- **ESPAsyncWebServer**: Async web server
- **Adafruit NeoPixel**: LED control
- **PubSubClient**: MQTT client
- **Aedes**: MQTT broker for Node.js

### Development Tools
- **CoreIOT**: IoT platform (app.coreiot.io)
- **JustGage**: Dashboard gauges
- **Font Awesome**: Icons

---

**Project Maintainer**: 
- [Phuong Tran](https://github.com/thp-tran)
- [Quang Tran](https://github.com/quntran)
- [Luc Quan]([https://github.com/Beoo2002)

For questions, please open an issue on GitHub.
