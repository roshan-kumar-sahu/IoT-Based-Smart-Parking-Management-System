# 🚗 IoT Based Smart Parking Management System
An IoT-enabled Smart Parking Management System developed using ESP32 for real-time parking slot monitoring, automatic gate control, parking billing, cloud analytics, and mobile dashboard monitoring using Blynk IoT and ThingSpeak.

<p align="center">
  <img src="Images/PROJECT_BANNER_2.png" width="100%">
</p>

<p align="center">

<img src="https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white"/>

<img src="https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white"/>

<img src="https://img.shields.io/badge/Blynk-IoT-23C48E?style=for-the-badge"/>

<img src="https://img.shields.io/badge/ThingSpeak-MATLAB-D9534F?style=for-the-badge"/>

<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white"/>

<img src="https://img.shields.io/badge/License-MIT-blue?style=for-the-badge"/>

</p>

<h3 align="center">

🏆 First Prize Winner – Internship Project Competition 2026

</h3>
--

An **IoT-enabled Smart Parking Management System** developed using **ESP32**, designed to automate vehicle entry/exit, monitor parking slot occupancy in real time, and provide cloud-based monitoring through **Blynk IoT** and **ThingSpeak**.

---

## 📌 Project Overview

This project provides a smart solution for parking management by automatically detecting available parking slots, controlling the entry gate using a servo motor, displaying parking information on an LCD, and updating data to cloud platforms for remote monitoring.

---

# 📷 Project Prototype

![Project Prototype](Images/PROJECT_IMAGE.jpeg)

> Final Working Prototype during Internship Project Demonstration
---

# ⚡ Features

- 🚗 Automatic Vehicle Entry & Exit Detection
- 🅿️ Real-Time Parking Slot Monitoring
- 📱 Blynk IoT Dashboard
- ☁️ ThingSpeak Cloud Analytics
- 🔐 Automatic Servo Gate Control
- 📟 16x2 I2C LCD Display
- 🔊 Passive Buzzer Alerts
- 🌈 RGB LED Slot Status Indication
- 💰 Parking Billing System (Demo)
- 📊 Parking History Tracking
- 🔄 Simulation Mode for Testing
- ⚙️ Non-Blocking State Machine Programming

---

# 🛠️ Hardware Components

- ESP32 DevKit V1
- 16x2 I2C LCD Display
- SG90 Servo Motor
- IR Sensors
- Passive Buzzer
- RGB LEDs
- Breadboards
- External 5V Power Supply
- Jumper Wires
- USB Cable

---

# 💻 Software Used

- Arduino IDE
- Blynk IoT
- ThingSpeak
- ESP32 Board Package
- C++

---
# 📊 System Architecture Diagram

![System Architecture Diagram](Images/SYSTEM_ARCHITECTURE.png)

# 📊 Blynk Web Dashboard

![Blynk Web Dashboard](Images/BLYNK_IOT_WEB_DASHBOARD.png)

---

# 📊 Blynk Mobile Dashboard

![Blynk Mobile Dashboard](Images/BLYNK_IOT_MOBILE_DASHBOARD.jpeg)

---

# ☁️ ThingSpeak Dashboard

![ThingSpeak Dashboard](Images/THINGSPEAK_PAGE_1.png)


![ThingSpeak Dashboard](Images/THINGSPEAK_PAGE_2.png)


![ThingSpeak Dashboard](Images/THINGSPEAK_PAGE_3.png)

---

# 🔄 System Flowchart

![Flowchart](Images/FINAL_FLOW_CHAT.png)

---

# 📂 Project Structure

```
IoT-Based-Smart-Parking-Management-System
│
├── Images
├── Source_Code
├── Documentation
├── Flowchart
├── README.md
└── LICENSE
```

---

# 🚀 How to Run

1. Install Arduino IDE
2. Install ESP32 Board Package
3. Install Required Libraries:
   - Blynk
   - ThingSpeak
   - ESP32Servo
   - LiquidCrystal_I2C
4. Open the project in Arduino IDE
5. Configure:
   - Wi-Fi SSID & Password
   - Blynk Auth Token
   - ThingSpeak API Key
6. Upload the code to ESP32
7. Power the hardware and monitor the dashboard
8. Give 5v external voltage to I2C LCD display and servo motor

---

# 📈 Future Improvements

- RFID Authentication
- QR Code Based Parking
- Automatic Online Payment
- Mobile Application
- AI-Based Parking Prediction
- Camera-Based Vehicle Detection
- Firebase Database Integration

---

# 👨‍💻 Author

**Roshan Kumar Sahu**

Electronics & Telecommunication Engineering

Parala Maharaja Engineering College (PMEC),Berhampur

---

# 📜 License

This project is licensed under the MIT License.

---

⭐ If you like this project, don't forget to star the repository!
