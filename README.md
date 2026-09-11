# 🚗 Smart Park System

> An ESP32-based smart parking prototype that detects parking availability, authenticates vehicles with RFID/NFC cards, opens the gate automatically, and sends parking information through Telegram.

![ESP32](https://img.shields.io/badge/ESP32-IoT-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-Framework-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Embedded-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Telegram](https://img.shields.io/badge/Telegram-Bot-26A5E4?style=for-the-badge&logo=telegram&logoColor=white)

## 📌 Overview

**Smart Park System** is an IoT parking management prototype built around an **ESP32**. The system is designed to reduce unnecessary searching for parking spaces by detecting available slots and providing the driver with a specific parking location before entering.

The current prototype supports **two parking slots (A1 and A2)** and combines sensors, RFID/NFC authentication, an OLED display, LEDs, a servo-controlled gate, Wi-Fi connectivity, and Telegram notifications.

The main idea is simple:

```text
Detect Parking Availability
          ↓
     Scan RFID Card
          ↓
    Authenticate User
          ↓
 ┌──────────────────────┐
 │ Slot Available?      │
 └──────────┬───────────┘
            │
      ┌─────┴─────┐
     YES          NO
      ↓             ↓
Show Slot        Show FULL
      ↓             ↓
Open Gate      Telegram Alert
      ↓
Telegram Notification
      ↓
Driver Goes to Assigned Slot
```

## ✨ Key Features

### 🅿️ Real-Time Parking Detection

Two IR sensors monitor parking slots **A1** and **A2**. The ESP32 determines whether each slot is occupied and calculates the number of available spaces.

### 🪪 RFID/NFC Authentication

An **MFRC522 RC522** reader scans the user's card UID. Only registered UIDs are allowed to continue through the parking entrance flow.

### 🚧 Automatic Gate Control

When an authenticated user has an available parking slot, a servo motor opens the gate for approximately 3 seconds and then closes it automatically.

### 📺 OLED Parking Display

A 128×64 SSD1306 OLED displays:

- A1 status
- A2 status
- Number of available slots
- Available parking location
- Welcome message
- Full parking notification
- Access denied notification

### 💡 Status Indicators

Two LEDs provide a simple visual indication:

| LED | Condition |
|---|---|
| 🟢 Green | At least one parking slot is available |
| 🔴 Red | All parking slots are occupied |

### 📱 Telegram Notifications

The system can send notifications through a Telegram bot for:

- Successful parking access
- Assigned parking slot
- Full parking condition
- Unknown RFID/NFC card

### 📡 Wi-Fi Connectivity

The ESP32 connects to Wi-Fi during startup and includes a basic reconnection mechanism when the connection is lost.

## 🧩 Hardware Components

| Component | Function |
|---|---|
| **ESP32 DevKit** | Main microcontroller and connectivity |
| **2× IR Sensor** | Detect occupancy of parking slots A1 and A2 |
| **MFRC522 RC522** | RFID/NFC card reader |
| **0.96" OLED SSD1306** | Display parking information |
| **Servo Motor** | Controls the parking gate |
| **Green LED** | Indicates available parking |
| **Red LED** | Indicates full parking |
| **Breadboard & Jumper Wires** | Prototype connections |

## 🔌 Pin Configuration

The current firmware uses the following GPIO configuration:

| Component | ESP32 GPIO | Purpose |
|---|---:|---|
| OLED SDA | 21 | I²C data |
| OLED SCL | 22 | I²C clock |
| IR Sensor A1 | 32 | Slot A1 detection |
| IR Sensor A2 | 33 | Slot A2 detection |
| Green LED | 26 | Available indicator |
| Red LED | 27 | Full indicator |
| Servo | 25 | Gate control |
| RC522 SS/SDA | 5 | SPI chip select |
| RC522 RST | 4 | RFID reset |

The OLED uses I²C address `0x3C` in the current implementation.

## ⚙️ Software & Libraries

The firmware is written in **C++ for ESP32/Arduino** and currently uses:

- `WiFi.h`
- `WiFiClientSecure.h`
- `UniversalTelegramBot.h`
- `ArduinoJson.h`
- `SPI.h`
- `MFRC522.h`
- `Wire.h`
- `Adafruit_GFX.h`
- `Adafruit_SSD1306.h`
- `ESP32Servo.h`

Install the required libraries through the Arduino IDE Library Manager or your preferred Arduino/ESP32 development environment.

## 🔄 System Workflow

### 1. System Startup

The ESP32 initializes the OLED, servo, RC522 reader, LEDs, and Wi-Fi connection.

### 2. Parking Detection

The IR sensors continuously check the condition of A1 and A2.

```text
A1 → KOSONG / TERISI
A2 → KOSONG / TERISI
        ↓
Calculate Available Slots
```

### 3. User Authentication

A driver scans an RFID/NFC card.

```text
RFID Card
    ↓
Read UID
    ↓
Check Registered UID
    ↓
┌───────────────┐
│ Valid?        │
└───────┬───────┘
        │
   ┌────┴────┐
  YES        NO
   ↓          ↓
Continue   Access Denied
```

### 4. Slot Validation

If the user is authenticated, the system checks parking availability.

- **Available:** display the available slot, notify through Telegram, and open the gate.
- **Full:** display `PENUH` and send a full-parking notification.

### 5. Gate Control

The servo rotates to approximately 90° to open the gate, waits approximately 3 seconds, then returns to 0°.

## 📱 Example User Experience

### Successful Access

```text
User scans RFID card
        ↓
   Card recognized
        ↓
  Slot A1 available
        ↓
OLED: WELCOME
      User Name
      Slot: A1
        ↓
Gate opens
        ↓
Telegram notification
        ↓
Driver proceeds to A1
```

### Parking Full

```text
User scans RFID card
        ↓
   Card recognized
        ↓
A1 = TERISI
A2 = TERISI
        ↓
OLED: PENUH
        ↓
Telegram: Parkiran sedang penuh
```

### Unknown Card

```text
Unknown RFID card
        ↓
OLED: AKSES DITOLAK
        ↓
Telegram security notification
        ↓
Gate remains closed
```

## 📂 Project Structure

```text
Smart-Park-System/
├── FinalSmartPark.ino   # Main ESP32 firmware
└── README.md             # Project documentation
```

## 🚀 Getting Started

### Prerequisites

- Arduino IDE or another ESP32-compatible development environment
- ESP32 board package
- ESP32 DevKit board
- Required hardware components
- Wi-Fi network
- Telegram bot for notifications

### Setup

1. Clone this repository.
2. Open `FinalSmartPark.ino` in Arduino IDE.
3. Install the required libraries.
4. Select the appropriate ESP32 board and COM port.
5. Configure your Wi-Fi and Telegram credentials using a secure method.
6. Connect the hardware according to the pin configuration above.
7. Upload the firmware to the ESP32.
8. Open Serial Monitor at **115200 baud**.
9. Test the parking sensors, RFID cards, OLED, servo, LEDs, and Telegram notifications.

## 🔐 Security Note

**Do not commit Wi-Fi passwords, Telegram bot tokens, or other credentials to a public repository.** The current firmware contains credentials directly in the source code because it was developed as a prototype.

Before using this project beyond a local prototype, rotate any credentials that have already been exposed and move secrets into a separate configuration file or another secure mechanism that is excluded from Git.

For a cleaner portfolio version, consider using a structure such as:

```text
secrets.h          # Local only — do not commit
secrets.example.h  # Safe template for GitHub
```

## 🧠 What This Project Demonstrates

This project is a practical example of combining several IoT concepts into one embedded system:

- ESP32 microcontroller programming
- Digital sensor input
- GPIO and peripheral management
- I²C communication with OLED
- SPI communication with RC522
- RFID/NFC authentication
- Servo motor control
- Wi-Fi connectivity
- Telegram Bot integration
- Real-time status display
- Basic event-driven system logic
- Hardware/software integration

## 🔮 Future Improvements

The prototype can be extended into a more production-oriented smart parking system by adding:

- 📊 More parking slots
- 🌐 Web/mobile dashboard
- 🗺️ Visual parking map
- 🗄️ Database for users and parking history
- 👤 Dynamic user registration instead of hard-coded UIDs
- 🔐 Secure credential management
- 📈 Parking usage analytics
- 🚘 Vehicle entry/exit tracking
- ⏱️ Parking duration tracking
- 📷 License plate recognition
- 📡 MQTT or REST API integration
- 🔄 Non-blocking servo and Wi-Fi logic

## 🎯 Project Goal

The long-term goal of **Smart Park System** is to provide a simple and affordable smart parking solution that can inform users about available parking spaces before they enter and streamline the entrance process through automated authentication and gate control.

## 👨‍💻 Author

**Archellyno**  
Information Technology Student — Universitas Udayana

---

⭐ Built as an IoT/Embedded Systems project using ESP32.