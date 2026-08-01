#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define I2C_SDA 17
#define I2C_SCL 18

Adafruit_INA219 ina219;

class CoulombCounter {
private:
    float totalCapacity_mAh;     
    float currentSoC_percent;    
    unsigned long lastUpdateTime;

public:
    CoulombCounter(float capacity_mAh) {
        this->totalCapacity_mAh = capacity_mAh;
        this->currentSoC_percent = 100.0; 
        this->lastUpdateTime = millis();
    }

    void update(float current_mA) {
        unsigned long currentTime = millis();
        
        float hoursPassed = (currentTime - lastUpdateTime) / 3600000.0; 
        
        float capacityDrained_mAh = current_mA * hoursPassed;
        
        float percentageDrained = (capacityDrained_mAh / totalCapacity_mAh) * 100.0;
        currentSoC_percent -= percentageDrained;
        
        if (currentSoC_percent < 0) currentSoC_percent = 0;
        
        lastUpdateTime = currentTime;
    }

    float getPercentage() {
        return currentSoC_percent;
    }
};

CoulombCounter myBattery(2600.0);

float simulatedVoltage = 4.20; 
float simulatedCurrent = 500.0; 

void setup() {
  Serial.begin(115200);
  delay(3000); 
  
  Wire.begin(I2C_SDA, I2C_SCL);
  ina219.begin(); 
  Serial.println("--- SMART BMS SIMULATION STARTED ---");
}

void loop() {
  simulatedVoltage -= 0.001; 
  myBattery.update(simulatedCurrent);

  Serial.print("Voltage: "); 
  Serial.print(simulatedVoltage, 3); 
  Serial.print(" V  |  Current Draw: "); 
  Serial.print(simulatedCurrent, 1); 
  Serial.print(" mA  |  Battery: ");
  Serial.print(myBattery.getPercentage(), 4);
  Serial.println(" %");

  delay(1000);
}