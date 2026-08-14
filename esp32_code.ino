#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>

// --- LoRa Pins ---
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

// --- Outputs ---
#define BUZZER_PIN 26
#define RED_LED_PIN 27

// --- WiFi ---
const char* ssid = "itel";
const char* password = "mywificode";

// --- Firebase ---
#define FIREBASE_PROJECT_ID "favour-sawmill-monitorin-62eaf"
#define FIREBASE_API_KEY "AIzaSyCyTzs_TItKOtDo65NQWEhVIQHWpQEUtNc"

// --- Threshold ---
#define ALERT_THRESHOLD 100

unsigned long lastLedToggle = 0;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // --- Connect WiFi ---
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n❌ WiFi failed!");
  }
  
  // --- LoRa Setup ---
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa init failed!");
    while(1);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  
  Serial.println("✅ LoRa Ready!");
  Serial.println("-----------------------------------");
}

void loop() {
  // --- Read LoRa Packet ---
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }
    
    // Parse data
    int commaIndex = received.indexOf(',');
    if (commaIndex > 0) {
      int pm25 = received.substring(0, commaIndex).toInt();
      int fanStatus = received.substring(commaIndex + 1).toInt();
      
      bool isCritical = (pm25 > ALERT_THRESHOLD);
      
      // --- 1. PRINT TO SERIAL MONITOR ---
      Serial.println("=================================");
      Serial.println("📥 RECEIVED DATA:");
      Serial.print("   PM2.5: ");
      Serial.print(pm25);
      Serial.println(" µg/m³");
      Serial.print("   Fans: ");
      Serial.println(fanStatus ? "ON" : "OFF");
      Serial.print("   Status: ");
      Serial.println(isCritical ? "🔴 CRITICAL" : "🟢 SAFE");
      Serial.println("=================================");
      
      // --- 2. CONTROL BUZZER ---
      if (isCritical) {
        digitalWrite(BUZZER_PIN, HIGH);
        Serial.println("🔊 BUZZER: ON");
      } else {
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("🔇 BUZZER: OFF");
      }
      
      // --- 3. CONTROL RED LED ---
      if (isCritical) {
        if (millis() - lastLedToggle > 500) {
          ledState = !ledState;
          digitalWrite(RED_LED_PIN, ledState ? HIGH : LOW);
          lastLedToggle = millis();
        }
      } else {
        digitalWrite(RED_LED_PIN, LOW);
        ledState = false;
      }
      
      // --- 4. SEND TO FIREBASE ---
      if (WiFi.status() == WL_CONNECTED) {
        sendToFirebase(pm25, fanStatus, isCritical);
      } else {
        Serial.println("❌ WiFi disconnected, skipping Firebase");
      }
    }
  }
  
  delay(10); 
}

// --- HTTP POST to Firebase ---
void sendToFirebase(int pm25, int fanStatus, bool isCritical) {
  HTTPClient http;
  
  String url = "https://firestore.googleapis.com/v1/projects/" + 
               String(FIREBASE_PROJECT_ID) + 
               "/databases/(default)/documents/sawmill_readings?key=" + 
               String(FIREBASE_API_KEY);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  String jsonPayload = "{";
  jsonPayload += "\"fields\": {";
  jsonPayload += "\"sawmill\": {\"stringValue\": \"Favour Sawmill\"},";
  jsonPayload += "\"pm25\": {\"integerValue\": " + String(pm25) + "},";
  jsonPayload += "\"fan_active\": {\"integerValue\": " + String(fanStatus) + "},";
  jsonPayload += "\"alert\": {\"integerValue\": " + String(isCritical ? 1 : 0) + "}";
  jsonPayload += "}";
  jsonPayload += "}";
  
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    Serial.println("✅ Firebase HTTP: " + String(httpResponseCode));
  } else {
    Serial.println("❌ HTTP Error: " + String(httpResponseCode));
  }
  
  http.end();
}
