# Crowpanel_GUI

GUI firmware for the **Crowpanel 4.3” (SKU: DIS06043H)** running on ESP32-S3, built with **LVGL 8.3.11** and designed using **EEZ Studio**.

The system provides a graphical monitoring and control interface for a **signal jammer charging system**, displaying real-time voltage and current readings from:

* Solar input
* Charger
* Gun
* Start/Stop charging control

The ESP32-S3 communicates via UART with an Arduino Nano to receive measurement data.

---

## 🖥 Hardware

* **Display:** Crowpanel 4.3 inch
* **MCU:** ESP32-S3
* **Touch Controller:** XPT2046 (Resistive)
* **External MCU:** Arduino Nano (UART communication)

---

## 🧰 Software & Libraries

* **Arduino IDE:** 2.3.6 (Linux)
* **GUI Designer:** EEZ Studio
* **Graphics Framework:** LVGL
* **Touch Library:** XPT2046_Touchscreen
* **Display Driver:** GFX Library for Arduino

---

# 📂 Folder Structure

```
Crowpanel_GUI/
│
├── sketch_feb22a.ino        // Main Arduino entry point
│
├── actions.cpp              // Event handlers implementation
├── actions.h                // Event handler declarations
│
├── screens.c                // Screen creation (EEZ generated)
├── screens.h
│
├── styles.c                 // LVGL styles (EEZ generated)
├── styles.h
│
├── ui.c                     // Main UI initialization
├── ui.h
│
├── vars.h                   // Global LVGL object references
├── structs.h                // Custom structures
│
├── fonts.h                  // Custom font declarations
│
├── images.c                 // Image resources
├── images.h
├── ui_image_logo.c
├── ui_image_power_on.c
├── ui_image_p.c
│
├── touch.h                  // ⚠ IMPORTANT: Resistive touch handling
│
└── nano_uart/               // UART communication with Arduino Nano
```

---

# 🔴 Important File: `touch.h`

This file is **critical** for resistive touch functionality.

It handles:

* XPT2046 initialization
* SPI configuration
* Touch coordinate mapping
* Calibration
* LVGL input device driver registration

If touch is:

* Inverted
* Offset
* Not responding
* Triggering randomly

→ Debug `touch.h` first.

---

# 🔌 UART Communication (nano_uart)

The ESP32-S3 receives voltage and current values from an Arduino Nano via UART.

### Data Flow

```
Arduino Nano → UART → ESP32-S3 → LVGL UI
```

### Responsibilities

* Parse incoming numeric data
* Update LVGL labels dynamically
* Trigger state changes if required

---

# ⚙️ Arduino IDE Configuration (CRITICAL)

Use the following settings exactly:

### Board Settings

* **Board:** ESP32S3 Dev Module
* **USB CDC on Boot:** Disabled
* **CPU Frequency:** 240 MHz (WiFi)
* **Core Debug Level:** None
* **USB DFU on Boot:** Disabled
* **Erase All Flash Before Boot:** Enabled
* **Flash Mode:** DIO 80MHz
* **Flash Size:** 4MB (32Mb)
* **Partition Scheme:** Huge APP (3MB No OTA / 1MB SPIFFS)
* **PSRAM:** QSPI PSRAM
* **Upload Mode:** UART0 / Hardware CDC
* **JTAG Adapter:** Disabled
* **USB Firmware MSC on Boot:** Disabled

---

## 🧠 Core Configuration

* Arduino runs on **Core 1**
* LVGL events run on **Core 1**

This avoids cross-core instability and timing issues.

---

# 🖼 GUI Overview

Designed in EEZ Studio and exported for LVGL 8.3.11.

### Interface Features

* Real-time voltage display
* Real-time current display
* Solar monitoring section
* Charger monitoring section
* Gun monitoring section
* Start/Stop charging buttons
* Status indicators

---

# 🚀 Performance Notes

* LVGL performs smoothly at 240MHz with QSPI PSRAM enabled.
* “Erase All Flash Before Boot” prevents partition corruption.
* Huge APP partition avoids memory overflow.
* Resistive touch accuracy depends heavily on proper calibration.

---

# 🛠 Troubleshooting

### UI Cut Off / Wrong Orientation

* Verify screen width & height configuration
* Check display rotation settings
* Confirm driver configuration matches panel orientation

### Touch Issues

* Verify SPI pins
* Check XPT2046 wiring
* Recalibrate in `touch.h`
* Confirm pressure threshold

### Random Crashes / Reboots

* Ensure PSRAM is enabled
* Confirm partition scheme
* Erase flash before upload

---

# 📌 Requirements Summary

| Component           | Version       |
| ------------------- | ------------- |
| LVGL                | 8.3.11        |
| XPT2046_Touchscreen | 1.4           |
| GFX Library         | 1.2.8         |
| Arduino IDE         | 2.3.6 (Linux) |

---

# 📄 Project Purpose

This project provides a stable, real-time embedded GUI system for:

* Monitoring solar-powered charging
* Controlling signal jammer charging cycles
* Providing safe user interaction via resistive touch
* Displaying electrical measurements clearly and reliably

---

If needed, this project can be extended with:

* Data logging
* Graph plotting
* WiFi telemetry
* OTA update support
* Fault detection logic

---
