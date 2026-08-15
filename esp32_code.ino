#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <LoRa.h>

// --- LoRa Pins for ESP32 ---
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define BUZZER_PIN 26

// --- WiFi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- Firebase Credentials ---
#define FIREBASE_API_KEY "AIzaSyCyTzs_TItKOtDo65NQWEhVIQHWpQEUtNc"
#define FIREBASE_PROJECT_ID "favour-sawmill-monitorin-62eaf"

// --- Alert Threshold ---
#define ALERT_THRESHOLD 100

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected");
  
  // Configure Firebase
  config.api_key = FIREBASE_API_KEY;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Configure LoRa
  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa failed!");
    while(1);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  Serial.println("✅ LoRa Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) received += (char)LoRa.read();
    
    int comma = received.indexOf(',');
    if (comma > 0) {
      int pm25 = received.substring(0, comma).toInt();
      int fanStatus = received.substring(comma + 1).toInt();
      
      Serial.printf("PM2.5: %d µg/m³ | Fans: %s\n", pm25, fanStatus ? "ON" : "OFF");
      
      // Buzzer
      digitalWrite(BUZZER_PIN, pm25 > ALERT_THRESHOLD ? HIGH : LOW);
      
      // Send to Firebase
      if (Firebase.ready()) {
        FirebaseJson json;
        json.add("sawmill", "Favour Sawmill");
        json.add("pm25", pm25);
        json.add("fan_active", fanStatus);
        json.add("alert", pm25 > ALERT_THRESHOLD ? 1 : 0);
        
        String path = "sawmill_readings/" + String(millis());
        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", path, json)) {
          Serial.println("✅ Sent to Firebase");
        } else {
          Serial.println("❌ Firebase Error: " + fbdo.errorReason());
        }
      }
    }
  }
  delay(100);
}
