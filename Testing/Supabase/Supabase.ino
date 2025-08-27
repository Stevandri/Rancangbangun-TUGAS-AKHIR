#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Paypayy";
const char* password = "12341234";

const char* supabaseUrl = "https://mdiburfsglepbivwydww.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1kaWJ1cmZzZ2xlcGJpdnd5ZHd3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTUzNjEwMTIsImV4cCI6MjA3MDkzNzAxMn0.kASF0Xuv4lvlzdNXfjvG_qIzDbkR56K0W14LmzR5LGk";

const char* databaseTable = "test_data";

void setup() {
  Serial.begin(115200);

  // Koneksi ke WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Persiapan JSON untuk dikirim
    StaticJsonDocument<200> doc;
    doc["message"] = "Hello from ESP32!";
    String jsonOutput;
    serializeJson(doc, jsonOutput);

    // Persiapan request HTTP POST
    String url = String(supabaseUrl) + "/rest/v1/" + String(databaseTable);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabaseAnonKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
    http.addHeader("Prefer", "return=minimal");

    Serial.print("Sending data to Supabase...");
    int httpResponseCode = http.POST(jsonOutput);

    if (httpResponseCode > 0) {
      Serial.printf("\nHTTP Response Code: %d\n", httpResponseCode);
      if (httpResponseCode == HTTP_CODE_CREATED) {
        Serial.println("Data successfully sent to Supabase!");
      } else {
        String payload = http.getString();
        Serial.println("Error sending data.");
        Serial.println(payload);
      }
    } else {
      Serial.printf("\nHTTP Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected.");
  }

  delay(5000); // Tunggu 5 detik sebelum mengirim data lagi
}