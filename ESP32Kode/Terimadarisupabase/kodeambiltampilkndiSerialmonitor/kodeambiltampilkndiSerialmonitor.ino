// ================================================================
//          MENERIMA DATA DARI SUPABASE KE ESP32
// ================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Pastikan library ini sudah di-install

// --- Konfigurasi WiFi ---
const char* ssid = "Stev"; // Ganti dengan nama WiFi Anda
const char* password = "12341234"; // Ganti dengan password WiFi Anda

// --- Konfigurasi Supabase ---
// Gunakan URL dan Key dari proyek Anda sebelumnya
const String supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
const String supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";

// Fungsi untuk mengambil data dari Supabase
void getDataFromSupabase() {
  // 1. Cek koneksi WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Koneksi WiFi terputus.");
    return;
  }

  HTTPClient http;
  
  // 2. Buat URL untuk mengambil data
  // Format: /rest/v1/{nama_tabel}?select={nama_kolom}
  String url = supabaseUrl + "/rest/v1/titipan?select=kode_ambil";
  
  Serial.println("Mengambil data dari URL: " + url);
  http.begin(url);

  // 3. Tambahkan header otentikasi
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);

  // 4. Lakukan request GET
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) { // Kode 200 berarti sukses
    String payload = http.getString();
    Serial.println("Berhasil mendapatkan data:");
    // Serial.println(payload); // Baris ini untuk debug jika perlu

    // 5. Proses data JSON yang diterima
    DynamicJsonDocument doc(2048); // Alokasikan memori untuk JSON
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("Gagal memproses JSON, error: ");
      Serial.println(error.c_str());
      return;
    }

    // 6. Tampilkan setiap nilai 'kode_ambil'
    Serial.println("=====================================");
    Serial.println("Daftar Kode Ambil dari Supabase:");
    for (JsonObject item : doc.as<JsonArray>()) {
      // Ambil nilai dari kolom "kode_ambil"
      const char* kode_ambil = item["kode_ambil"]; 
      Serial.println(kode_ambil);
    }
    Serial.println("=====================================");

  } else {
    Serial.print("Gagal mengambil data. Kode error HTTP: ");
    Serial.println(httpResponseCode);
  }

  // 7. Tutup koneksi
  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nMemulai program...");

  // Koneksi ke WiFi
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
  // Panggil fungsi untuk mengambil data setiap 10 detik
  getDataFromSupabase();
  
  Serial.println("\nMenunggu 10 detik sebelum mengambil data lagi...");
  delay(10000); 
}