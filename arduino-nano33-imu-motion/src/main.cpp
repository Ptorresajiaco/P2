#include <Arduino.h>
#include <Arduino_LSM9DS1.h>

#define MOTION_THRESHOLD 1.5
#define MOTION_DURATION 1000
#define STOP_DELAY 5000

bool ledState = false;
bool motionConfirmed = false;
bool ledBlinking = false;
unsigned long motionStartTime = 0;
unsigned long lastMotionTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 100;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  if (IMU.begin()) {
    Serial.println("IMU initialized");
  } else {
    Serial.println("Failed to initialize IMU");
    while (1);
  }
  
  Serial.println("State: LED OFF - Waiting for motion");
}

void loop() {
  float x, y, z;
  float acceleration;
  unsigned long currentTime = millis();
  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    acceleration = sqrt(x*x + y*y + z*z);
    
    if (acceleration > MOTION_THRESHOLD) {
      if (motionStartTime == 0) {
        motionStartTime = currentTime;
        Serial.println("State: Motion detected");
      }
      lastMotionTime = currentTime;
      
      if (!motionConfirmed && (currentTime - motionStartTime >= MOTION_DURATION)) {
        motionConfirmed = true;
        ledBlinking = true;
        Serial.println("State: LED BLINKING - Motion confirmed >1.5g for 1s");
      }
    } else {
      motionStartTime = 0;
    }
  }
  
  if (ledBlinking) {
    if (currentTime - lastMotionTime >= STOP_DELAY) {
      ledBlinking = false;
      motionConfirmed = false;
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("State: LED OFF - No motion for 5s");
    } else {
      if (currentTime - lastBlinkTime >= blinkInterval) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
        lastBlinkTime = currentTime;
      }
    }
  }
}
