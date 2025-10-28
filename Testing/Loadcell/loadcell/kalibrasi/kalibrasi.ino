#include "HX711.h"

// ---------------- PIN MAPPING ----------------
const int LOADCELL_DOUT_PIN = 35;  // Ganti sesuai wiring kamu
const int LOADCELL_SCK_PIN  = 18;  // Ganti sesuai wiring kamu

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Tare… hapus beban dari timbangan");
  delay(5000);
  scale.tare();
  Serial.println("Tare selesai.");
  Serial.println("Letakkan benda dengan berat diketahui (misalnya 100 g) …");
  delay(5000);
  
  long reading = scale.get_units(10);  // ambil rata-rata 10 pembacaan
  Serial.print("Hasil pembacaan: ");
  Serial.println(reading);
  
  // Cetak petunjuk agar user menghitung faktor kalibrasi:
  Serial.println("Gunakan rumus: calibration_factor = reading / known_weight");
  Serial.println("(Contoh: jika reading = 50000 dan berat = 100 g -> faktor = 50000 / 100 = 500)");
}

void loop() {
  // tidak digunakan dalam tahap kalibrasi
}
