#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

#define RED_PIN 13    
#define GREEN_PIN 12  
#define BLUE_PIN 14   


#define LEDC_CHANNEL_RED   0
#define LEDC_CHANNEL_GREEN 1
#define LEDC_CHANNEL_BLUE  2
#define LEDC_TIMER_BIT     8
#define LEDC_BASE_FREQ     5000   

void setup(){
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  ledcSetup(LEDC_CHANNEL_RED, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(LEDC_CHANNEL_GREEN, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(LEDC_CHANNEL_BLUE, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(RED_PIN, LEDC_CHANNEL_RED);
  ledcAttachPin(GREEN_PIN, LEDC_CHANNEL_GREEN);
  ledcAttachPin(BLUE_PIN, LEDC_CHANNEL_BLUE);
  ledcWrite(LEDC_CHANNEL_RED, 0);
  ledcWrite(LEDC_CHANNEL_GREEN, 0);
  ledcWrite(LEDC_CHANNEL_BLUE, 0);
  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  float lux = lightMeter.readLightLevel();
  
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  float lightThreshold = 100.0; 
  if (lux < lightThreshold) {
    ledcWrite(LEDC_CHANNEL_RED, 255);  
    ledcWrite(LEDC_CHANNEL_GREEN, 255); 
    ledcWrite(LEDC_CHANNEL_BLUE, 255);
  } else {
    ledcWrite(LEDC_CHANNEL_RED, 0);
    ledcWrite(LEDC_CHANNEL_GREEN, 0);
    ledcWrite(LEDC_CHANNEL_BLUE, 0);
  }
  delay(1000);  
}