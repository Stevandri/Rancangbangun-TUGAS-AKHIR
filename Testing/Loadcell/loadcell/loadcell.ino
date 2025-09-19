//================================================================
//          BABAK 1, POIN 1: SENSOR BERAT (2x HX711)
//================================================================

// Library yang dibutuhkan
#include "HX711.h"

// --- Konfigurasi Pin Sensor Berat #1 ---
const int DOUT_PIN_1 = 35;
const int SCK_PIN_1 = 18;

// --- Konfigurasi Pin Sensor Berat #2 ---
const int DOUT_PIN_2 = 39;
const int SCK_PIN_2 = 19;

// Inisialisasi objek untuk setiap sensor
HX711 scale1;
HX711 scale2;

// Variabel untuk kalibrasi
// PENTING: Nilai ini HARUS disesuaikan dengan load cell Anda.
// Mulailah dengan -7050, lalu letakkan beban yang diketahui (misal 1kg),
// dan ubah nilai ini hingga pembacaan menjadi 1.00.
// Variabel Kalibrasi
float KALIBRASI_B1 = -7050.0;
float KALIBRASI_B2 = -7050.0;



void setup() {
  Serial.begin(115200);
  Serial.println("--- Memulai Poin 1: Inisialisasi Sensor Berat ---");

  // Inisialisasi Sensor 1
  scale1.begin(DOUT_PIN_1, SCK_PIN_1);
  scale1.set_scale(KALIBRASI_B1);
  scale1.tare(); // Reset pembacaan awal menjadi 0
  Serial.println("Sensor Berat 1 Siap.");

  // Inisialisasi Sensor 2
  scale2.begin(DOUT_PIN_2, SCK_PIN_2);
  scale2.set_scale(KALIBRASI_B2);
  scale2.tare(); // Reset pembacaan awal menjadi 0
  Serial.println("Sensor Berat 2 Siap.");
  
  Serial.println("\n--- Memulai Pembacaan ---");
}

void loop() {
  // Baca berat dari sensor 1
  float berat1 = scale1.get_units(5); // Ambil rata-rata 5 pembacaan
  Serial.print("Berat beban 1 : ");
  Serial.print(berat1, 2); // Tampilkan 2 angka di belakang koma
  Serial.println(" kg");
delay(1000); // Jeda 1 detik agar mudah dibaca
  // Baca berat dari sensor 2
  float berat2 = scale2.get_units(10); // Ambil rata-rata 5 pembacaan
  Serial.print("Berat beban 2 : ");
  Serial.print(berat2, 2); // Tampilkan 2 angka di belakang koma
  Serial.println(" kg");

  Serial.println("--------------------");
  
  delay(1000); // Jeda 1 detik agar mudah dibaca
}