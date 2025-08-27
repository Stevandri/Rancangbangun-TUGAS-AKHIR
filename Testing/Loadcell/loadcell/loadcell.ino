#include "HX711.h"

// Pin ke HX711
#define DT 19   // DOUT dari HX711
#define SCK 27  // CLK dari HX711

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(DT, SCK);

  Serial.println("Inisialisasi...");
  delay(1000);

  if (scale.is_ready()) {
    Serial.println("HX711 Siap.");
    scale.set_scale();  // Kalibrasi nanti
    scale.tare();       // Reset berat awal ke 0
  } else {
    Serial.println("HX711 tidak terdeteksi.");
    while (1);
  }
}

void loop() {
  Serial.print("Berat: ");
  Serial.print(scale.get_units(), 2); // Menampilkan 2 angka di belakang koma
  Serial.println(" kg");
  delay(1000);
}
