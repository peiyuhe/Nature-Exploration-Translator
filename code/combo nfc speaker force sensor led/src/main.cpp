#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Audio.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>

Audio audio;              
WiFiClientSecure client;  


const char* ssid = "sample_ssid";//you need to be replaced with the actual ssid used
const char* password = "sample_password";//you need to be replaced with the password key used
const char* openai_host = "api.openai.com";
const int httpsPort = 443;
const char* openaiApiKey = "sample_key";//you need to be replaced with the actual key used

// I2S Audio Output Pins
#define I2S_BCK_IO 26
#define I2S_WS_IO 25
#define I2S_DO_IO 22

// NFC Configuration
#define SS_PIN 21  
#define RST_PIN 34  
MFRC522 mfrc522(SS_PIN, RST_PIN); 

// NFC Key and Block
MFRC522::MIFARE_Key key;
const int block = 4; 
// Pressure Sensor and LED Pins
const int forceSensorPins[] = {36, 39}; 
const int greenLEDPins[] = {13, 2};
const int redLEDPins[] = {14, 0};

// System State Variables
String storedNFCData[2] = {"", ""};  
int associatedSensor[2] = {-1, -1};  
bool isAudioPlaying = false;
bool isPressed[2] = {false, false};   
bool dataReady[2] = {false, false};   
int firstPressedSensor = -1;          
const int sensorThreshold = 300;

void connectToWiFi();
void readNFC();
void checkPressureSensors();
bool readFromCard(byte *buffer, byte &bufferSize);
void resetNFC();
void changeLEDColor(int index, bool isGreen);
String requestOpenAI(String prompt);
void playDescription(const String &description);

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  audio.setPinout(I2S_BCK_IO, I2S_WS_IO, I2S_DO_IO);
  audio.setVolume(100);

  SPI.begin(18, 19, 23, SS_PIN);
  mfrc522.PCD_Init();   

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  for (int i = 0; i < 2; i++) {
    pinMode(greenLEDPins[i], OUTPUT);
    pinMode(redLEDPins[i], OUTPUT);
    digitalWrite(greenLEDPins[i], LOW);
    digitalWrite(redLEDPins[i], LOW);
  }

  Serial.println("System initialized. Waiting for NFC card...");
}

void loop() {
  readNFC();
  checkPressureSensors();
  audio.loop();  
}

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");
}

void readNFC() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    byte buffer[18];
    byte bufferSize = sizeof(buffer);
    if (readFromCard(buffer, bufferSize)) {
      String nfcData = "";
      for (int i = 0; i < 16; i++) {
        nfcData += (char)buffer[i];
      }
      
      Serial.print("NFC Data Stored: ");
      Serial.println(nfcData);

      // Store the NFC data for the first available sensor slot
      for (int i = 0; i < 2; i++) {
        if (storedNFCData[i] == "") {
          storedNFCData[i] = nfcData;
          associatedSensor[i] = i;
          dataReady[i] = true;
          Serial.print("Associated NFC data with sensor ");
          Serial.println(i);
          break;
        }
      }
    }
    resetNFC();
  }
}

void checkPressureSensors() {
  if (isAudioPlaying) return;

  for (int i = 0; i < 2; i++) {
    if (!dataReady[i]) continue;

    int sensorValue = analogRead(forceSensorPins[i]);

    if (sensorValue > sensorThreshold && !isPressed[i]) {
      isPressed[i] = true;

      if (firstPressedSensor == -1) {
        firstPressedSensor = i;

        digitalWrite(greenLEDPins[i], HIGH);  
        delay(2000);
        digitalWrite(greenLEDPins[i], LOW);   
        Serial.println("Requesting description from OpenAI...");
        String description = requestOpenAI(storedNFCData[i]);
        if (!description.isEmpty()) {
          isAudioPlaying = true;
          playDescription(description);
          isAudioPlaying = false;
        }
      } else {
        if (i == firstPressedSensor) {
          digitalWrite(greenLEDPins[i], HIGH);
          delay(2000);
          digitalWrite(greenLEDPins[i], LOW);
        } else {
          digitalWrite(redLEDPins[i], HIGH);
          delay(2000);
          digitalWrite(redLEDPins[i], LOW); 
        }
        firstPressedSensor = -1;
      }
      isPressed[i] = false;
    }
  }
}

bool readFromCard(byte *buffer, byte &bufferSize) {
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("NFC Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  status = mfrc522.MIFARE_Read(block, buffer, &bufferSize);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("NFC Read failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  return true;
}

void resetNFC() {
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

String requestOpenAI(String prompt) {
  client.setInsecure();
  if (!client.connect(openai_host, httpsPort)) {
    Serial.println("Connection to OpenAI API failed!");
    return "";
  }

  String requestBody = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"You are a helpful assistant.\"},{\"role\":\"user\",\"content\":\"Describe the color and shape of the following item in less than 15 words: " + prompt + "\"}],\"max_tokens\":30}";
  
  String request = String("POST /v1/chat/completions HTTP/1.1\r\n") +
                   "Host: " + openai_host + "\r\n" +
                   "Authorization: Bearer " + String(openaiApiKey) + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + String(requestBody.length()) + "\r\n" +
                   "\r\n" +
                   requestBody;

  client.print(request);
  
  String response = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  while (client.available()) {
    response += (char)client.read();
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("JSON Parsing Error: ");
    Serial.println(error.c_str());
    return "";
  }

  const char* content = doc["choices"][0]["message"]["content"];
  return content ? String(content) : "";
}

void playDescription(const String &description) {
  if (!description.isEmpty()) {
    audio.connecttospeech(description.c_str(), "en");
    delay(500);
    while (audio.isRunning()) {
      audio.loop();
    }
  }
}

void changeLEDColor(int index, bool isGreen) {
  digitalWrite(greenLEDPins[index], LOW);
  digitalWrite(redLEDPins[index], LOW);
  
  if (isGreen) {
    digitalWrite(greenLEDPins[index], HIGH);   
  } else {
    digitalWrite(redLEDPins[index], HIGH);     
  }
}
 
