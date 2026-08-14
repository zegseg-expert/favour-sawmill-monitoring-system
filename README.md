# 🌲 Favour Sawmill Monitoring System

A real-time air quality monitoring system built for **Favour Sawmill** using IoT.

## 🚀 System Features
- **Sharp GP2Y1014 Dust Sensor** (PM2.5 detection)
- **Automatic Fan Control** (Turns on at 100 µg/m³)
- **LoRa 433MHz Wireless Communication** (Long-range transmission)
- **ESP32 Receiver** with:
  - 🔊 Buzzer Alarm
  - 🔴 Blinking Red LED Alert
  - 📶 WiFi Connectivity
- **Firebase Firestore Database** (Cloud storage)
- **Live Web Dashboard** on GitHub Pages (Shows last 10 readings)

## 🔧 Hardware
- **Transmitter:** Arduino Nano + Dust Sensor + LoRa + 2x DC Fans
- **Receiver:** ESP32 + LoRa + Buzzer + Red LED

## 🌐 Live Dashboard
[https://zegseg-expert.github.io/favour-sawmill-monitoring-system/](https://zegseg-expert.github.io/favour-sawmill-monitoring-system/)

## 📡 How it Works
1. Transmitter reads dust levels & controls fans.
2. Transmitter sends data via **LoRa 433MHz**.
3. ESP32 Receiver gets data, sounds buzzer, blinks LED.
4. ESP32 sends data to **Firebase via HTTP REST**.
5. GitHub Pages dashboard fetches data & displays it live.

## 🛠️ Built With
- Arduino IDE
- Firebase Firestore
- GitHub Pages
- Chart.js
