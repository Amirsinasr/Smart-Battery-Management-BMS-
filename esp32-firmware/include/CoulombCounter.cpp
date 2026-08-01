#pragma once
#include <Arduino.h>

class CoulombCounter {
private:
    float totalCapacity_mAh;     
    float currentSoC_percent;    
    unsigned long lastUpdateTime;

public:
    CoulombCounter(float capacity_mAh) {
        this->totalCapacity_mAh = capacity_mAh;
        this->currentSoC_percent = 0.0;
        this->lastUpdateTime = 0;
    }

    /**
     * @brief Estimates the initial battery percentage based on voltage when the device turns on.
     * This relies on a simple Open Circuit Voltage (OCV) curve for a standard Lithium-Ion battery.
     */
    void initialize(float initialVoltage_V) {
        if (initialVoltage_V >= 4.2) {
            currentSoC_percent = 100.0;
        } else if (initialVoltage_V <= 3.0) {
            currentSoC_percent = 0.0;
        } else {
            currentSoC_percent = ((initialVoltage_V - 3.0) / (4.2 - 3.0)) * 100.0;
        }
        
        lastUpdateTime = millis();
    }

    /**
     * @brief The core integration math. Call this every time you read the INA219.
     * @param current_mA The instantaneous current reading (positive = discharging, negative = charging).
     */
    void update(float current_mA) {
        unsigned long currentTime = millis();
        
        if (lastUpdateTime == 0) {
            lastUpdateTime = currentTime;
            return;
        }


        float deltaTime_hours = (currentTime - lastUpdateTime) / 3600000.0;

        float capacityDrawn_mAh = current_mA * deltaTime_hours;

        float percentageDrawn = (capacityDrawn_mAh / totalCapacity_mAh) * 100.0;

        currentSoC_percent -= percentageDrawn;

        if (currentSoC_percent > 100.0) currentSoC_percent = 100.0;
        if (currentSoC_percent < 0.0)   currentSoC_percent = 0.0;

        lastUpdateTime = currentTime;
    }

    float getPercentage() {
        return currentSoC_percent;
    }
};