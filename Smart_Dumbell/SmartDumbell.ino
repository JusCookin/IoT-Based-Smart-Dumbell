#define BLYNK_TEMPLATE_ID "TMPL3KNYaO6av"
#define BLYNK_TEMPLATE_NAME "Smart Dumbell"
#define BLYNK_AUTH_TOKEN "ozHX5uf59-w5WqU3tLFosbi05Jwa1QsO"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30100_PulseOximeter.h"  // Added for oximeter

char ssid[] = "Agash";      
char pass[] = "password";

MPU6050 mpu;
// OLED display setup (typically 128x64 pixels)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1   // Reset pin (-1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pulse oximeter setup
PulseOximeter pox;
uint32_t lastBeatDetectedTime = 0;
float heartRate = 0;
float spO2 = 0;
bool heartBeat = false;

int repCount = 0;
int setCount = 0;
bool lifting = false;
BlynkTimer timer;
unsigned long lastAxisPrint = 0;  // For tracking when to print axis data
bool forceBlynkUpdate = false;    // Flag to force Blynk update

// Callback function for beat detection
void onBeatDetected() {
    lastBeatDetectedTime = millis();
    heartBeat = true;
    Serial.println("Beat detected!");
}

// Function to send data to Blynk and update OLED
void sendDataToBlynk() {
    // Update pulse oximeter readings
    pox.update();
    
    // Get the latest values if they're valid (greater than 0)
    if (pox.getHeartRate() > 0) {
        heartRate = pox.getHeartRate();
    }
    if (pox.getSpO2() > 0) {
        spO2 = pox.getSpO2();
    }
    
    // Always update Blynk with current rep, set, and HR counts
    Blynk.virtualWrite(V1, repCount);
    Blynk.virtualWrite(V2, setCount);
    Blynk.virtualWrite(V3, heartRate);  // Adding heart rate to Blynk
    Blynk.virtualWrite(V4, spO2);       // Adding SpO2 to Blynk
    
    // Debug message to confirm Blynk updates
    if (forceBlynkUpdate) {
        Serial.println("===> BLYNK UPDATE SENT <===");
        Serial.print("Sent to Blynk - Reps: ");
        Serial.print(repCount);
        Serial.print(", Sets: ");
        Serial.print(setCount);
        Serial.print(", HR: ");
        Serial.print(heartRate, 0);
        Serial.print(", SpO2: ");
        Serial.println(spO2, 0);
        forceBlynkUpdate = false;
    }
    
    // Update OLED display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // First row - Reps and HR
    display.setCursor(0, 0);
    display.print("Reps: ");
    display.print(repCount);
    display.setCursor(70, 0);
    display.print("HR: ");
    if (heartRate > 0) {
        display.print(heartRate, 0);
    } else {
        display.print("--");
    }
    
    // Second row - Sets and SpO2
    display.setCursor(0, 16);
    display.print("Sets: ");
    display.print(setCount);
    display.setCursor(70, 16);
    display.print("SpO2: ");
    if (spO2 > 0) {
        display.print(spO2, 0);
        display.print("%");
    } else {
        display.print("--");
    }
    
    // Get motion data
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    // Third row - Z value
    display.setCursor(0, 32);
    display.print("Z: ");
    display.print(az);
    
    // Fourth row - Status
    display.setCursor(0, 48);
    display.print("Status: ");
    display.print(lifting ? "UP" : "DOWN");
    
    // Heart indicator in the corner
    if (millis() - lastBeatDetectedTime < 500) {  // Flash for 500ms on beat
        display.fillCircle(120, 48, 5, SSD1306_WHITE);
    } else {
        display.drawCircle(120, 48, 5, SSD1306_WHITE);
    }
    
    display.display();
    
    // Reset heartbeat flag
    heartBeat = false;
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize OLED display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        while(1);
    }
    display.clearDisplay();
    
    // Display welcome message
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Hello!");
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.println("Smart Dumbell");
    display.setCursor(0, 40);
    display.println("with Heart Rate");
    display.display();
    
    Serial.println("Hello! Smart Dumbell with HR Starting...");
    delay(2000);  // Show welcome message for 2 seconds
    
    // Clear display after initial message
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Connecting WiFi");
    display.display();
    
    // Connect to WiFi and Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    
    // Wait a bit to ensure proper Blynk connection
    delay(1000);
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Connected");
    display.display();
    delay(1000);
    
    // Initialize MAX30100 sensor
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Init Heart Sensor...");
    display.display();
    
    // Initialize pulse oximeter
    if (!pox.begin()) {
        Serial.println("MAX30100 Not Found!");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("HR Sensor Error!");
        display.display();
        delay(2000);
        // Continue without heart rate sensor
    } else {
        Serial.println("MAX30100 Ready!");
        pox.setOnBeatDetectedCallback(onBeatDetected);
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    }
    
    // Initialize MPU6050
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Init Motion Sensor...");
    display.display();
    
    mpu.initialize();
    if (!mpu.testConnection()) {
        Serial.println("MPU6050 Not Found!");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Motion Sensor Error!");
        display.display();
        while (1);
    }
    
    Serial.println("MPU6050 Ready!");
    Serial.println("Smart Dumbell with HR Monitor");
    Serial.println("----------------------");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Ready to start!");
    display.setCursor(0, 16);
    display.println("Lift to count");
    display.setCursor(0, 32);
    display.println("Measuring HR & SpO2");
    display.display();
    delay(1500);
    
    // Initial display of reps and sets
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Reps: 0  HR: --");
    display.setCursor(0, 16);
    display.println("Sets: 0  SpO2: --%");
    display.display();

    // Set Blynk timer to send data every 300ms for better responsiveness
    timer.setInterval(300L, sendDataToBlynk);  
    
    // Initial Blynk update to make sure the app shows zeros
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
    Serial.println("Initial values sent to Blynk");
    
    // Debug to check Blynk connection
    if (Blynk.connected()) {
        Serial.println("Blynk is connected successfully!");
    } else {
        Serial.println("Warning: Blynk connection issue!");
    }
}

// Function to manually sync all values to Blynk
void syncBlynk() {
    Blynk.virtualWrite(V1, repCount);
    Blynk.virtualWrite(V2, setCount);
    Blynk.virtualWrite(V3, heartRate);
    Blynk.virtualWrite(V4, spO2);
    Serial.println("Manual Blynk sync performed");
}

// Add handler for Blynk connection event
BLYNK_CONNECTED() {
    Serial.println("Blynk connected!");
    // Sync all values immediately when connected
    syncBlynk();
}

void loop() {
    Blynk.run();
    timer.run();
    
    // Update pulse oximeter
    pox.update();

    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Print all axis data every 200ms to avoid flooding the serial monitor
    unsigned long currentMillis = millis();
    if (currentMillis - lastAxisPrint >= 200) {
        lastAxisPrint = currentMillis;
        
        // Print all accelerometer and gyroscope values
        Serial.print("Accel (x,y,z): ");
        Serial.print(ax); Serial.print(", ");
        Serial.print(ay); Serial.print(", ");
        Serial.print(az);
        
        Serial.print(" | Gyro (x,y,z): ");
        Serial.print(gx); Serial.print(", ");
        Serial.print(gy); Serial.print(", ");
        Serial.print(gz);
        
        Serial.print(" | Status: ");
        Serial.print(lifting ? "UP" : "DOWN");
        
        Serial.print(" | Reps: ");
        Serial.print(repCount);
        
        Serial.print(" | Sets: ");
        Serial.print(setCount);
        
        Serial.print(" | HR: ");
        Serial.print(heartRate, 0);
        
        Serial.print(" | SpO2: ");
        Serial.println(spO2, 0);
    }

    // EXTREMELY REDUCED threshold for easy counting
    if (az > 1500 && !lifting) {  // Reduced from 3000 to 1500
        lifting = true;  // Movement detected (lifting)
        Serial.println("===> MOVEMENT UP DETECTED! <===");
    }
    if (az < 1000 && lifting) {  // Reduced from 2000 to 1000
        repCount++;      // Count one rep
        lifting = false; // Reset for next rep

        Serial.println("===> REP COUNTED! <===");
        Serial.print("Reps: ");
        Serial.print(repCount);
        Serial.print(" | Sets: ");
        Serial.println(setCount);
        
        // Update OLED display immediately after a rep
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Reps: ");
        display.print(repCount);
        display.setCursor(70, 0);
        display.print("HR: ");
        if (heartRate > 0) {
            display.print(heartRate, 0);
        } else {
            display.print("--");
        }
        
        display.setCursor(0, 16);
        display.print("Sets: ");
        display.print(setCount);
        display.setCursor(70, 16);
        display.print("SpO2: ");
        if (spO2 > 0) {
            display.print(spO2, 0);
            display.print("%");
        } else {
            display.print("--");
        }
        
        display.setCursor(0, 32);
        display.print("Z: ");
        display.println(az);
        display.setCursor(0, 48);
        display.println("REP COUNTED!");
        display.display();
        
        // Send data to Blynk immediately after rep - DIRECT UPDATE
        Blynk.virtualWrite(V1, repCount);
        // Set flag to force Blynk update in timer function too
        forceBlynkUpdate = true;

        // Convert reps into sets (8 reps = 1 set)
        if (repCount >= 8) {
            setCount++;
            repCount = 0;
            
            // Update serial output for completed set
            Serial.println("===> SET COMPLETED! <===");
            
            // Flash completed message on OLED
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(0, 8);
            display.println("SET");
            display.setCursor(0, 32);
            display.println("COMPLETED!");
            display.display();
            delay(1500);
            
            // Return to normal display with heart rate
            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.print("Reps: 0");
            display.setCursor(70, 0);
            display.print("HR: ");
            if (heartRate > 0) {
                display.print(heartRate, 0);
            } else {
                display.print("--");
            }
            
            display.setCursor(0, 16);
            display.print("Sets: ");
            display.print(setCount);
            display.setCursor(70, 16);
            display.print("SpO2: ");
            if (spO2 > 0) {
                display.print(spO2, 0);
                display.print("%");
            } else {
                display.print("--");
            }
            display.display();
            
            // Update Blynk with BOTH direct and timer approaches
            Blynk.virtualWrite(V1, repCount);
            Blynk.virtualWrite(V2, setCount);
            forceBlynkUpdate = true;
            
            // Extra measure - call manual sync
            syncBlynk();
        }
    }

    // Periodically check Blynk connection
    static unsigned long lastCheck = 0;
    if (currentMillis - lastCheck >= 5000) {  // Every 5 seconds
        lastCheck = currentMillis;
        if (!Blynk.connected()) {
            Serial.println("Blynk connection lost! Attempting to reconnect...");
            Blynk.connect();  // Attempt to reconnect
        }
    }

    delay(10);  // Small delay to prevent CPU overload
}
