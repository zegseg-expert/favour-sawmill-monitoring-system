# 🌲 Favour Sawmill Monitoring System

A real-time air quality monitoring system for sawmill dust detection using IoT.

## Features
- Real-time PM2.5 dust monitoring
- Automatic fan control
- LoRa 433MHz wireless communication
- Buzzer alert system
- Live dashboard on GitHub Pages
- Firebase Firestore database

## Hardware
- Transmitter: Arduino Nano + Sharp GP2Y1014 + LoRa + 2x DC Fans
- Receiver: ESP32 + LoRa + Buzzer

## Live Dashboard
Visit: `https://YOUR_USERNAME.github.io/favour-sawmill-monitoring-system`

## How it works
1. Transmitter senses dust and sends data via LoRa
2. ESP32 receiver gets data and forwards to Firebase
3. Dashboard fetches data from Firebase and displays it live

## License
MIT
