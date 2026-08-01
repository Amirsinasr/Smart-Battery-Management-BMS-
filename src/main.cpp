#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#define I2C_SDA 17
#define I2C_SCL 18

Adafruit_INA219 ina219;

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);
  
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip. Check your wiring!");
    while (1) { delay(10); } 
  }
  
  Serial.println("INA219 Found and successfully communicating on pins 17 and 18!");
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}