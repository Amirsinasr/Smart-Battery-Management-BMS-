#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "iPhone";
const char* password = "00000000";

const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "smartbms/battery/Sina"; 

#define I2C_SDA 17
#define I2C_SCL 18
Adafruit_INA219 ina219;
WiFiClient espClient;
PubSubClient client(espClient);

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

void setup_wifi() {
  Serial.print("\nConnecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nSUCCESS: Wi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0, 10000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("CONNECTED!");
    } else {
      Serial.print("FAILED, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000); 
  
  Wire.begin(I2C_SDA, I2C_SCL);
  ina219.begin(); 
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  simulatedVoltage -= 0.001; 
  myBattery.update(simulatedCurrent);

  String payload = "{";
  payload += "\"voltage\": " + String(simulatedVoltage, 3) + ", ";
  payload += "\"current\": " + String(simulatedCurrent, 1) + ", ";
  payload += "\"soc\": " + String(myBattery.getPercentage(), 4);
  payload += "}";

  client.publish(mqtt_topic, payload.c_str());

  Serial.print("Published: ");
  Serial.println(payload);

  delay(1000);
}