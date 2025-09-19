// ================================================================
//               KONEKSI WIFI, SENSOR & DATABASE
// ================================================================

#include "HX711.h"
#include <WiFi.h>
#include <HTTPClient.h>

// --- Konfigurasi WiFi ---
const char* ssid = "Stev";
const char* password = "12341234";

// --- Konfigurasi Supabase ---
String supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
String supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";
String laci1Id = "299a21dc-afac-4078-816c-2454adfb34f2";
String laci2Id = "a9bcb51c-c979-4417-952b-871ce500071b";

// --- Konfigurasi Pin Sensor Berat ---
const int DOUT_PIN_1 = 35;
const int SCK_PIN_1 = 18;
const int DOUT_PIN_2 = 39;
const int SCK_PIN_2 = 19;

// Inisialisasi objek untuk setiap sensor
HX711 scale1;
HX711 scale2;

// Variabel Kalibrasi
float KALIBRASI_B1 = -7050.0;
float KALIBRASI_B2 = -7050.0;

void setup() {
  Serial.begin(115200);

  // Inisialisasi Sensor Berat 1
  scale1.begin(DOUT_PIN_1, SCK_PIN_1);
  scale1.set_scale(KALIBRASI_B1);
  scale1.tare();
  Serial.println("Sensor Berat 1 Siap.");

  // Inisialisasi Sensor Berat 2
  scale2.begin(DOUT_PIN_2, SCK_PIN_2);
  scale2.set_scale(KALIBRASI_B2);
  scale2.tare();
  Serial.println("Sensor Berat 2 Siap.");

  // Koneksi WiFi
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Baca berat dari sensor 1 dan 2
    float berat1 = scale1.get_units(5);
    float berat2 = scale2.get_units(5);
    
    Serial.print("Berat beban 1: ");
    Serial.print(berat1, 2);
    Serial.println(" kg");
    
    Serial.print("Berat beban 2: ");
    Serial.print(berat2, 2);
    Serial.println(" kg");

    // Kirim data ke Supabase
    sendToSupabase(laci1Id, berat1);
    sendToSupabase(laci2Id, berat2);

  } else {
    Serial.println("WiFi terputus. Mencoba menghubungkan kembali...");
    WiFi.begin(ssid, password);
  }

  delay(5000); // Tunggu 5 detik sebelum pembacaan dan pengiriman berikutnya
}

void sendToSupabase(String id, float berat) {
  HTTPClient http;
  String url = supabaseUrl + "/rest/v1/laci?select=berat_titipan&id=eq." + id;
  
  String jsonPayload = "{\"berat_titipan\": " + String(berat, 2) + "}";

  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Prefer", "return=minimal");

  int httpResponseCode = http.PATCH(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.print("Kode Respon HTTP untuk laci ");
    Serial.print(id);
    Serial.print(": ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Gagal terhubung ke Supabase untuk laci ");
    Serial.print(id);
    Serial.print(". Kode error: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}