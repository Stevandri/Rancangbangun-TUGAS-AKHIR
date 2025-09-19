// ================================================================
//   KONEKSI WIFI & SENSOR MAGNETIK (PIN 34/36 + RESISTOR EKSTERNAL)
// ================================================================

#include <WiFi.h>
#include <HTTPClient.h>

// --- Konfigurasi WiFi ---
const char* ssid = "Stev";
const char* password = "12341234";

// --- Konfigurasi Supabase ---
const String supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
const String supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";
const String laci1Id = "299a21dc-afac-4078-816c-2454adfb34f2";
const String laci2Id = "a9bcb51c-c979-4417-952b-871ce500071b";

// --- Konfigurasi Pin Sensor Magnetik ---
const int MAGNETIK_PIN_1 = 36;
const int MAGNETIK_PIN_2 = 34;

// Variabel untuk menyimpan status terakhir dari setiap pintu
bool statusLaci1_sebelumnya;
bool statusLaci2_sebelumnya;

void connectToWiFi() {
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

void setup() {
  Serial.begin(115200);

  // ====================== PERUBAHAN UTAMA ======================
  // Menggunakan INPUT karena pin 34 & 36 tidak punya pull-up internal
  // dan kita mengandalkan resistor eksternal.
  pinMode(MAGNETIK_PIN_1, INPUT);
  pinMode(MAGNETIK_PIN_2, INPUT);
  // =============================================================
  
  Serial.println("Sensor Magnetik Siap.");

  connectToWiFi();

  // Inisialisasi status awal pintu
  statusLaci1_sebelumnya = (digitalRead(MAGNETIK_PIN_1) == LOW);
  statusLaci2_sebelumnya = (digitalRead(MAGNETIK_PIN_2) == LOW);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus. Mencoba menghubungkan kembali...");
    connectToWiFi();
  }

  // --- Pembacaan Sensor Magnetik ---
  // Logika untuk resistor PULL-UP: LOW jika magnet dekat (pintu tertutup).
  bool laci1_tertutup = (digitalRead(MAGNETIK_PIN_1) == LOW);
  bool laci2_tertutup = (digitalRead(MAGNETIK_PIN_2) == LOW);

  // Kirim data ke Supabase HANYA JIKA ada perubahan status
  if (laci1_tertutup != statusLaci1_sebelumnya) {
    Serial.print("Status Laci 1 berubah! Sekarang: ");
    Serial.println(laci1_tertutup ? "Tertutup" : "Terbuka");
    sendToSupabase(laci1Id, laci1_tertutup);
    statusLaci1_sebelumnya = laci1_tertutup;
  }

  if (laci2_tertutup != statusLaci2_sebelumnya) {
    Serial.print("Status Laci 2 berubah! Sekarang: ");
    Serial.println(laci2_tertutup ? "Tertutup" : "Terbuka");
    sendToSupabase(laci2Id, laci2_tertutup);
    statusLaci2_sebelumnya = laci2_tertutup;
  }

  delay(200);
}

void sendToSupabase(const String& id, bool tertutup) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  HTTPClient http;
  String url = supabaseUrl + "/rest/v1/laci?id=eq." + id;
  String jsonPayload = String("{\"magnetik_door\": ") + (tertutup ? "true" : "false") + "}";

  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Prefer", "return=minimal");

  int httpResponseCode = http.PATCH(jsonPayload);

  if (httpResponseCode > 0 && (httpResponseCode == 200 || httpResponseCode == 204)) {
    Serial.print("Berhasil update Supabase untuk laci ");
    Serial.print(id);
    Serial.println(".");
  } else {
    Serial.print("Gagal update Supabase untuk laci ");
    Serial.print(id);
    Serial.print(". Kode error: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}