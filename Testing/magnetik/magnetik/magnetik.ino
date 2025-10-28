// ================================================
//  PROGRAM SEDERHANA: MONITORING MAGNETIC DOOR
//  Tanpa LED, hanya tampil di Serial Monitor
// ================================================

#include <Arduino.h>

// ---------------- PIN SENSOR ----------------
const int MAGNET1_PIN = 36;   // Sensor magnetic pintu 1
const int MAGNET2_PIN = 34;   // Sensor magnetic pintu 2

// ---------------- VARIABEL STATUS ----------------
bool door1_closed = false;
bool door2_closed = false;

unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL = 1000; // tampil tiap 1 detik

void setup() {
  Serial.begin(115200);

  // Setup pin input sensor magnetic (gunakan pull-up internal)
  pinMode(MAGNET1_PIN, INPUT);
  pinMode(MAGNET2_PIN, INPUT);

  Serial.println("=== Monitoring Magnetic Door Sensor ===");
  Serial.println("Menampilkan status pintu setiap 1 detik...");
}

void loop() {
  // Baca sensor magnetic
  // Asumsi: LOW = tertutup (magnet menempel), HIGH = terbuka
  door1_closed = (digitalRead(MAGNET1_PIN) == LOW);
  door2_closed = (digitalRead(MAGNET2_PIN) == LOW);

  // Cetak status ke Serial Monitor setiap 1 detik
  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();

    Serial.println("\n---- STATUS PINTU ----");
    Serial.print("Pintu 1 : ");
    Serial.println(door1_closed ? "TERTUTUP" : "TERBUKA");
    Serial.print("Pintu 2 : ");
    Serial.println(door2_closed ? "TERTUTUP" : "TERBUKA");
    Serial.println("----------------------");
  }
}
