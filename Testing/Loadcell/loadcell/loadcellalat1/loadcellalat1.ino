//================================================================
//        SENSOR BERAT (1x HX711, 1 Load Cell)
//================================================================

#include "HX711.h"

// --- Konfigurasi Pin HX711 ---
const int DOUT_PIN = 35;   // Data out (DOUT) ke ESP32
const int SCK_PIN  = 18;   // Serial clock (SCK) ke ESP32

// Inisialisasi HX711
HX711 scale;

// Variabel untuk kalibrasi
// Mulai dengan -7050, lalu letakkan beban diketahui (misal 1kg),
// ubah hingga hasil mendekati 1.00.
float KALIBRASI = -7050.0;

void setup() {
  Serial.begin(115200);
  Serial.println("--- Inisialisasi Sensor Berat ---");

  // Inisialisasi HX711
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(KALIBRASI);
  scale.tare(); // Nolkan pembacaan awal
  Serial.println("HX711 siap.\n");
}

void loop() {
  // Baca berat (ambil rata-rata dari 10 sampel)
  float berat = scale.get_units(10);
  
  Serial.print("Berat beban : ");
  Serial.print(berat, 2);  // 2 angka di belakang koma
  Serial.println(" kg");

  delay(1000); // Jeda 1 detik
}
