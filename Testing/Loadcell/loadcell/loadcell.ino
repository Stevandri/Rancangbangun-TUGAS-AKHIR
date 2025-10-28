#include "HX711.h"

// ---------------- PIN MAPPING ----------------
const int LOADCELL1_DOUT = 35;
const int LOADCELL1_SCK  = 18;
const int LOADCELL2_DOUT = 39;
const int LOADCELL2_SCK  = 19;

HX711 scale1;
HX711 scale2;

// ---------------- KALIBRASI ----------------
// Ganti nilai ini dengan hasil kalibrasi dari proses kalibrasi sebelumnya
float cal1 = -44938.971;   // contoh hasil kalibrasi load cell 1 (gram)
float cal2 = -104.586;   // contoh hasil kalibrasi load cell 2 (gram)

// ---------------- PENGATURAN PEMBACAAAN ----------------
const int NUM_SAMPLES = 20;   // jumlah pembacaan untuk rata-rata
const int SAMPLE_DELAY = 20;   // jeda antar pembacaan (ms) → 20 ms = total ~2 detik per 100 pembacaan

void setup() {
  Serial.begin(115200);

  // Inisialisasi Load Cell 1
  scale1.begin(LOADCELL1_DOUT, LOADCELL1_SCK);
  scale1.set_scale(cal1);
  scale1.tare();

  // Inisialisasi Load Cell 2
  scale2.begin(LOADCELL2_DOUT, LOADCELL2_SCK);
  scale2.set_scale(cal2);
  scale2.tare();

  Serial.println("== Pembacaan Berat Load Cell 1 & 2 (Rata-rata 100x, satuan gram) ==");
}

void loop() {
  // Pembacaan Load Cell 1
  if (scale1.is_ready()) {
    float total1 = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
      total1 += scale1.get_units(1);
      delay(SAMPLE_DELAY);
    }
    float weight1 = total1 / NUM_SAMPLES;
    Serial.print("Berat 1: ");
    Serial.print(weight1, 2);
    Serial.println(" g");
  } else {
    Serial.println("Load Cell 1 belum siap");
  }

  // Pembacaan Load Cell 2
  if (scale2.is_ready()) {
    float total2 = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
      total2 += scale2.get_units(1);
      delay(SAMPLE_DELAY);
    }
    float weight2 = total2 / NUM_SAMPLES;
    Serial.print("Berat 2: ");
    Serial.print(weight2, 2);
    Serial.println(" g");
  } else {
    Serial.println("Load Cell 2 belum siap");
  }

  // (Opsional) Total gabungan dua load cell
  Serial.println("-------------------------");
  delay(1000); // tampil setiap 1 detik setelah pembacaan selesai
}
