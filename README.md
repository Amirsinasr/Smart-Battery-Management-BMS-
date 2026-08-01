# Smart Battery Management System (BMS) Dashboard 🔋

![React](https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Espressif](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-3C5280?style=for-the-badge&logo=mqtt&logoColor=white)

A Full-Stack Internet of Things (IoT) Battery Management System. This project bridges physical hardware with a real-time web dashboard, utilizing an ESP32-S3 to track battery discharge via I2C, compute the State of Charge (SoC), and stream live telemetry over MQTT to a React frontend.

## 📺 Live Demo

![Demo of Dashboard and Hardware](https://github.com/Amirsinasr/Smart-Battery-Management-BMS-/blob/main/BMS-demo.gif)

## 🏗️ System Architecture

This project is divided into two distinct environments to separate the embedded firmware from the web application:

1. **Hardware Edge Node (ESP32-S3 + INA219):** Reads raw voltage and current data via the I2C protocol.
2. **Firmware Logic (C++):** A custom Coulomb Counter class tracks milliamp-hours (mAh) drained over time to calculate an accurate percentage, rather than relying on raw voltage.
3. **Transport Layer (MQTT):** Telemetry is serialized into JSON and published to a HiveMQ broker over 2.4GHz Wi-Fi.
4. **Client Interface (React):** Subscribes to the MQTT topic via Secure WebSockets (`wss://`) and renders real-time dynamic graphs using Recharts.

## ✨ Key Engineering Features

* **Real-Time Coulomb Counting:** Implements a time-based integration algorithm to calculate true battery capacity (SoC) instead of basic voltage mapping.
* **Hardware-in-the-Loop Simulation:** Features a software-simulated battery drain mode. This allowed for full unblocked frontend UI/UX development and testing prior to the physical integration of volatile lithium-ion cells.
* **Network Resilience:** The embedded C++ firmware includes a timeout watchdog. If the Wi-Fi or MQTT broker drops, the ESP32 automatically breaks blocking loops and triggers a safe hardware restart to self-heal the connection.
* **Micro-Data Visualization:** The React frontend automatically re-scales the Y-axis and disables smoothing animations to accurately visualize fractional percentage drops (0.001%) in real-time.

## 🧰 Hardware Bill of Materials (BOM)

* **Microcontroller:** ESP32-S3 (DevKitC-1)
* **Sensor:** INA219 High-Side DC Current/Voltage Sensor
* **Target Load:** 18650 Lithium-Ion Cell (3.7V / 2600mAh)

### I2C Wiring Map

| INA219 Pin | ESP32-S3 Pin | Purpose |
| :--- | :--- | :--- |
| **VCC** | 3.3V | Logic Power |
| **GND** | GND | Common Ground |
| **SDA** | GPIO 17 | I2C Data |
| **SCL** | GPIO 18 | I2C Clock |

---

## 🚀 Getting Started

### 1. Embedded Firmware Setup
1. Open the `esp32-firmware` folder in VS Code using the **PlatformIO** extension.
2. Open `main.cpp` and update the Wi-Fi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
