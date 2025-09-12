//================================================================
//          BABAK 1, POIN 2: SENSOR BERAT + GPS
//          (Koneksi GPS satu arah: TX GPS -> RX ESP32)
//================================================================

// Library yang dibutuhkan
#include "HX711.h"         // Untuk sensor berat
#include <TinyGPS++.h>     // Untuk GPS

// --- Konfigurasi Pin Sensor Berat #1 (SAMA) ---
const int DOUT_PIN_1 = 16;
const int SCK_PIN_1 = 4;

// --- Konfigurasi Pin Sensor Berat #2 (SAMA) ---
const int DOUT_PIN_2 = 18;
const int SCK_PIN_2 = 5;

// --- Konfigurasi Pin GPS ---
// Kita hanya butuh koneksi dari TX GPS ke pin RX ESP32.
const int GPS_RX_PIN = 19; // Terhubung ke TX Modul GPS
const int GPS_TX_PIN = 17; // Pin ini tidak perlu disambungkan secara fisik

// Inisialisasi objek
HX711 scale1;
HX711 scale2;
TinyGPSPlus gps; // Objek untuk GPS

// Variabel Kalibrasi (SAMA)
float KALIBRASI_B1 = -7050.0;
float KALIBRASI_B2 = -7050.0;


void setup() {
  Serial.begin(115200);
  Serial.println("--- Memulai Poin 2: Inisialisasi Sensor Berat + GPS ---");

  // Inisialisasi Sensor Berat (SAMA seperti Poin 1)
  scale1.begin(DOUT_PIN_1, SCK_PIN_1);
  scale1.set_scale(KALIBRASI_B1);
  scale1.tare();
  Serial.println("Sensor Berat 1 Siap.");

  scale2.begin(DOUT_PIN_2, SCK_PIN_2);
  scale2.set_scale(KALIBRASI_B2);
  scale2.tare();
  Serial.println("Sensor Berat 2 Siap.");
  
  // Inisialisasi GPS
  // Fungsi ini tetap sama, walau pin TX ESP32 tidak terhubung.
  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("Modul GPS (Serial2) Siap.");

  Serial.println("\n--- Memulai Pembacaan ---");
}

void loop() {
  // Selalu proses data mentah dari GPS
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  // Tampilkan semua data sensor setiap 1 detik
  static unsigned long last_read_time = 0;
  if (millis() - last_read_time > 1000) { // Update setiap 1000 ms
    last_read_time = millis();

    // --- Cetak data Berat ---
    float berat1 = scale1.get_units(5);
    Serial.print("Berat beban 1 : ");
    Serial.print(berat1, 2);
    Serial.println(" kg");

    float berat2 = scale2.get_units(5);
    Serial.print("Berat beban 2 : ");
    Serial.print(berat2, 2);
    Serial.println(" kg");

    // --- Cetak data GPS ---
    if (gps.location.isUpdated() && gps.location.isValid()) {
      Serial.print("Lat : ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(", Long : ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Lokasi GPS belum didapat (mencari sinyal...)");
    }

    Serial.println("--------------------");
  }
}