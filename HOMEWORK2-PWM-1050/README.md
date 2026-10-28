# ESP32 Dual-LED Fading with PWM and OLED Telemetry

An embedded IoT project built on the ESP32 microcontroller that demonstrates independent hardware-accelerated Pulse Width Modulation (PWM) control across two LEDs with different frequencies, alongside real-time parameter monitoring on an SSD1306 I²C OLED display.

---

## 📋 Table of Contents
- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Wiring & Pinout Tables](#wiring--pinout-tables)
- [Core Theoretical Concepts](#core-theoretical-concepts)
- [Software Setup & Dependencies](#software-setup--dependencies)
- [Code Structure & Implementation](#code-structure--implementation)

---

## 🚀 Project Overview

This project is divided into two core operational tasks:
1. **Task 1 (Dual-LED Fading):** Configures the ESP32's internal LED Control (LEDC) peripheral to drive two separate LEDs on distinct GPIO pins at different PWM frequencies, smoothly fading them up and down while keeping their active duty cycles accessible in memory.
2. **Task 2 (OLED Telemetry Display):** Integrates an SSD1306 I²C OLED screen to render live telemetry data, displaying the configured PWM frequencies and real-time duty cycle percentages as the LEDs change brightness.

---

## 🛠️ Hardware Requirements

* **ESP32 Development Board** (NodeMCU-32S / ESP32-WROOM-32 or compatible)
* **2× Light Emitting Diodes (LEDs)**
* **2× Current-Limiting Resistors** ($220\,\Omega$ or $330\,\Omega$)
* **SSD1306 I²C OLED Display** (128x64 pixels, monochrome)
* **Breadboard and Jumper Wires**

---

## 🔌 Wiring & Pinout Tables

### 1. LED Circuit Wiring
| Component | Connection Pin / Rail | ESP32 GPIO / Power Pin | Description |
| :--- | :--- | :--- | :--- |
| **LED 1 Anode (+)** | Resistor Output | GPIO 16 (Digital Output) | Carries the PWM signal for LED 1. |
| **LED 1 Cathode (-)** | Ground Rail | GND | Completes the circuit for LED 1. |
| **LED 2 Anode (+)** | Resistor Output | GPIO 17 (Digital Output) | Carries the PWM signal for LED 2. |
| **LED 2 Cathode (-)** | Ground Rail | GND | Completes the circuit for LED 2. |

> ⚠️ **Important Note:** Avoid using strapping pins (such as GPIO 0, 2, 5, 12, or 15) or input-only pins (GPIO 34–39) for PWM outputs to prevent boot conflicts or lack of output drivers.

### 2. SSD1306 OLED Display Wiring (I²C)
| Component | OLED Pin | ESP32 Pin / Rail | Description |
| :--- | :--- | :--- | :--- |
| **Power** | VCC | 3.3V | Supplies operating voltage to the display module. |
| **Ground** | GND | GND | Completes the power loop. |
| **Data Line** | SDA | GPIO 21 | Carries serial data for the I²C communication bus. |
| **Clock Line** | SCL | GPIO 22 | Carries the serial clock signal for the I²C bus. |

---

## 📚 Core Theoretical Concepts

* **Pulse Width Modulation (PWM):** Simulates analog behavior by rapidly toggling a digital output between $\text{HIGH}$ ($3.3\text{V}$) and $\text{LOW}$ ($0\text{V}$).
  $$\text{Duty Cycle} = \frac{T_{on}}{T_{on} + T_{off}} \times 100\%$$
* **Apparent Brightness:** Due to human visual persistence, higher duty cycles deliver greater average power, increasing apparent brightness.
* **PWM Frequency:** Defines how many complete cycles occur per second ($f = \frac{1}{T}$). Frequencies between $1\text{ kHz}$ and $5\text{ kHz}$ guarantee flicker-free operation.
* **ESP32 LEDC Peripheral:** Unlike standard Arduino boards, the ESP32 utilizes dedicated timers and channels to manage independent PWM signals without locking up the CPU.

---

## 📦 Software Setup & Dependencies

To compile and upload this project using the **Arduino IDE**, ensure you have installed:
1. **ESP32 Board Support Package** via the Arduino Board Manager.
2. **Required Libraries** (install via Library Manager):
   - `Adafruit_SSD1306`
   - `Adafruit_GFX_Library`

---

## 💻 Code Implementation

Ensure your global variables for duty cycles are shared between the PWM fading logic and the display updater function so that on-screen telemetry matches hardware state precisely.
```cpp
// Example snippet tracking duty cycles for display synchronization
int led1_duty = 0;
int led2_duty = 0;