#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>

// =====================
// KONFIGURASI WIFI
// =====================
const char* ssid = "Stev";
const char* password = "12341234";

// =====================
// KONFIGURASI SUPABASE
// =====================
// Ganti dengan URL dan API Key kamu
const char* SUPABASE_URL = "https://swixpgqbndtgiyysaqrn.supabase.co";
const char* SUPABASE_API_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";

// =====================
// KONFIGURASI GPS
// =====================
HardwareSerial gpsSerial(2); // Gunakan UART2
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi tersambung ✅");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Menunggu data GPS...\n");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
    
    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      String waktu = gps.time.isValid()
                     ? String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second())
                     : "Unknown";

      // === Tampilkan di Serial Monitor ===
      Serial.println("========================");
      Serial.println("DATA GPS TERBACA ✅");
      Serial.print("Latitude : "); Serial.println(latitude, 6);
      Serial.print("Longitude: "); Serial.println(longitude, 6);
      Serial.print("Waktu    : "); Serial.println(waktu);
      Serial.println("========================\n");

      // === Kirim ke Supabase ===
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(SUPABASE_URL);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("apikey", SUPABASE_API_KEY);
        http.addHeader("Authorization", String("Bearer ") + SUPABASE_API_KEY);

        String jsonData = "{\"latitude\": " + String(latitude, 6) +
                          ", \"longitude\": " + String(longitude, 6) +
                          ", \"waktu\": \"" + waktu + "\"}";

        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
          Serial.println("Data terkirim ke Supabase ✅");
          Serial.print("Kode respons: ");
          Serial.println(httpResponseCode);
        } else {
          Serial.print("Gagal kirim data ❌ Kode: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      } else {
        Serial.println("WiFi terputus ❌");
      }

      delay(5000); // kirim tiap 5 detik
    }
  }
  Serial.println("Nyariii");
}
