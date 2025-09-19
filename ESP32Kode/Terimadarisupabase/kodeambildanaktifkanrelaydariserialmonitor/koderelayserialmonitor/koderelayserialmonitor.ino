// ================================================================
//      KONTROL RELAY BERBASIS KODE DARI SUPABASE
// ================================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Pastikan library ini sudah di-install

// --- Konfigurasi WiFi ---
const char* ssid = "Stev"; // Ganti dengan nama WiFi Anda
const char* password = "12341234"; // Ganti dengan password WiFi Anda

// --- Konfigurasi Supabase ---
const String supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
const String supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";

// --- Konfigurasi Hardware ---
const int RELAY_PIN_1 = 4;  // Relay untuk Laci 1 terhubung ke GPIO 4
const int RELAY_PIN_2 = 2;  // Relay untuk Laci 2 terhubung ke GPIO 2

// ID Loker sesuai dengan yang ada di database Supabase
const String LACI_1_ID = "299a21dc-afac-4078-816c-2454adfb34f2";
const String LACI_2_ID = "a9bcb51c-c979-4417-952b-871ce500071b";

// --- Struktur untuk menyimpan data ---
struct TitipanData {
  String kode_ambil;
  String loker_id;
};

const int MAX_KODE = 20; // Batas maksimal kode yang bisa disimpan
TitipanData dataTitipan[MAX_KODE];
int jumlahKodeTersimpan = 0;

// --- Pengaturan Waktu untuk Update Data ---
unsigned long waktuSebelumnya = 0;
const long intervalUpdate = 30000; // Update data dari Supabase setiap 30 detik

// ================================================================
//                      FUNGSI-FUNGSI
// ================================================================

// Fungsi untuk membuka laci (mengaktifkan relay)
void bukaLaci(int nomorLaci) {
  int pinRelay;

  if (nomorLaci == 1) {
    pinRelay = RELAY_PIN_1;
    Serial.println("-> Cocok! Membuka Laci 1...");
  } else if (nomorLaci == 2) {
    pinRelay = RELAY_PIN_2;
    Serial.println("-> Cocok! Membuka Laci 2...");
  } else {
    return; // Nomor laci tidak valid
  }

  digitalWrite(pinRelay, LOW);  // Aktifkan relay (Active LOW)
  delay(1000);                  // Biarkan terbuka selama 1 detik
  digitalWrite(pinRelay, HIGH); // Matikan relay
  Serial.println("-> Laci kembali terkunci.");
}

// Fungsi untuk mengambil dan menyimpan data dari Supabase
void updateDataFromSupabase() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  HTTPClient http;
  
  // Ambil kolom kode_ambil DAN loker_id
  String url = supabaseUrl + "/rest/v1/titipan?select=kode_ambil,loker_id";
  
  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    
    DynamicJsonDocument doc(4096); // Ukuran disesuaikan untuk data lebih banyak
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.println("Gagal memproses JSON!");
      return;
    }

    // Kosongkan data lama sebelum diisi data baru
    jumlahKodeTersimpan = 0;
    JsonArray array = doc.as<JsonArray>();

    // Simpan data baru ke dalam array struct
    for (JsonObject item : array) {
      if (jumlahKodeTersimpan < MAX_KODE) {
        dataTitipan[jumlahKodeTersimpan].kode_ambil = item["kode_ambil"].as<String>();
        dataTitipan[jumlahKodeTersimpan].loker_id = item["loker_id"].as<String>();
        jumlahKodeTersimpan++;
      }
    }
    Serial.print("Data berhasil diupdate. Total kode tersimpan: ");
    Serial.println(jumlahKodeTersimpan);

  } else {
    Serial.print("Gagal update data. Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// Fungsi untuk cek input dari Serial Monitor
void cekInputSerial() {
  if (Serial.available() > 0) {
    String inputKode = Serial.readStringUntil('\n');
    inputKode.trim(); // Hapus spasi atau karakter tak terlihat

    if (inputKode.length() > 0) {
      Serial.print("Mengecek kode: '");
      Serial.print(inputKode);
      Serial.println("'");
      
      bool kodeDitemukan = false;
      for (int i = 0; i < jumlahKodeTersimpan; i++) {
        if (inputKode.equalsIgnoreCase(dataTitipan[i].kode_ambil)) {
          kodeDitemukan = true;
          if (dataTitipan[i].loker_id == LACI_1_ID) {
            bukaLaci(1);
          } else if (dataTitipan[i].loker_id == LACI_2_ID) {
            bukaLaci(2);
          }
          break; // Hentikan pencarian jika kode sudah ditemukan
        }
      }

      if (!kodeDitemukan) {
        Serial.println("-> Kode salah atau tidak ditemukan.");
      }
    }
  }
}

// ================================================================
//                      SETUP & LOOP
// ================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\nMemulai Sistem Kontrol Loker...");

  // Setup pin relay sebagai OUTPUT dan set ke HIGH (non-aktif)
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);

  // Koneksi ke WiFi
  Serial.print("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");

  // Ambil data pertama kali saat startup
  updateDataFromSupabase();
  Serial.println("\nSistem siap. Silakan masukkan kode ambil pada Serial Monitor.");
}

void loop() {
  // 1. Cek input dari Serial Monitor secara terus-menerus
  cekInputSerial();

  // 2. Update data dari Supabase secara berkala (setiap 30 detik)
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= intervalUpdate) {
    waktuSebelumnya = waktuSekarang;
    Serial.println("\nMemperbarui data dari Supabase...");
    updateDataFromSupabase();
  }
}