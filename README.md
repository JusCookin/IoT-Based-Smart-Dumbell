🏋️‍♂️ Smart-Dumbbell-With-Repetition-and-Health-Monitoring
An intelligent IoT-based smart dumbbell that automatically counts exercise repetitions and sets using motion sensors, while simultaneously monitoring heart rate and SpO₂ levels. Powered by ESP32, this compact fitness tool provides real-time feedback via OLED display and is designed to enhance personal training, physiotherapy, and workout tracking.

🌟 Overview
This project introduces a smart dumbbell that combines physical exercise tracking with health monitoring. Using an MPU6050 gyroscope and a MAX30100 pulse oximeter, the system accurately detects reps and sets based on motion angles and displays heart rate and oxygen levels on a 0.96" OLED. Designed for standalone operation, the ESP32 microcontroller processes all data locally while maintaining portability and user-friendly interaction.

✨ Features
Automated Rep & Set Detection: Counts repetitions and sets based on motion angle thresholds.

Heart Rate & SpO₂ Monitoring: Real-time display of vitals using onboard sensors.

OLED Display Feedback: Shows reps, sets, and health metrics during exercise.

Gesture-Based Accuracy: Sensors integrated directly into dumbbell for precise motion capture.

Compact & Wireless: Operates on battery power with minimal hardware footprint.

Custom Thresholds: Adjustable settings for rep count and motion sensitivity.

User Engagement: Motivates users from beginner to expert with interactive feedback.

🔧 Hardware Components
ESP32 Development Board – Main processing and control unit

MPU6050 Gyroscope + Accelerometer – Detects angular motion to count reps

MAX30100 Pulse Oximeter Sensor – Measures heart rate and SpO₂

0.96" OLED Display (I2C) – Displays live workout and health data

Rechargeable Battery Unit – Ensures wireless, portable operation

📊 System Components
Motion Detection Algorithm: Identifies reps based on Z-axis motion patterns

Health Monitoring Pipeline: Processes sensor data to derive heart rate & SpO₂

ESP32 Firmware: Controls data flow between sensors and display

OLED UI: Cycles through rep count, set count, and vitals in real time

Hardware Architecture(Prototype):
![WhatsApp Image 2025-06-16 at 19 28 29_8311a103](https://github.com/user-attachments/assets/fd21cd9d-9c7f-4584-8012-6df8606f3f2c)

Blynk Interface:
![WhatsApp Image 2025-06-16 at 19 22 03_f6356855](https://github.com/user-attachments/assets/e19d9a69-20c1-435b-81ad-2d416ea4244e)
