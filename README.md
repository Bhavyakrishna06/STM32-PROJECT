# Smart Parking System 🚗

An automated, real-time parking management system designed to reduce urban traffic congestion and optimize parking space utilization. Developed as an academic project at St. Thomas College of Engineering & Technology.

## 👥 Project Team
* Anaswara G
* Bhavya Krishna
* Krithika K
* **Project Guide:** Ms. Priyanka Rajeev M

## 🛠️ Hardware & Software
* **Microcontroller:** STM32 (C)
* **Sensors:** Infrared (IR) Sensors for slot detection
* **Actuators:** 5V Servo Motor for automated barrier
* **Display:** 16x2 I2C LCD Display

## ⚙️ How It Works
1. **Real-Time Detection:** IR sensors continuously monitor individual parking slots.
2. **Automated Entry:** The entrance gate sensor detects arriving vehicles. If slots are available, the servo motor automatically opens the barrier.
3. **Live Status Display:** The I2C LCD at the entrance provides drivers with real-time updates on available slots, preventing unnecessary entry when the lot is full.

## 📂 Repository Contents
* `main.c`: Core microcontroller logic written in pure C.
* `Project_Microcontroller.pdf`: Official project abstract and documentation.
