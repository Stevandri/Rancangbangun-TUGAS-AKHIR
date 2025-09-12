//================================================================
//          BABAK 1 SELESAI: SEMUA SENSOR TERINTEGRASI
//================================================================

// Library yang dibutuhkan
#include "HX711.h"         // Untuk sensor berat
#include <TinyGPS++.h>     // Untuk GPS
#include <Wire.h>          // Tambahan untuk komunikasi I2C (RTC)
#include "RTClib.h"        // Tambahan untuk Modul RTC

// --- Inisialisasi Objek ---
RTC_DS3231 rtc; // Objek untuk RTC
HX711 scale1;
HX711 scale2;
TinyGPSPlus gps;

// --- Konfigurasi Pin Sensor Berat #1 ---
const int DOUT_PIN_1 = 35;
const int SCK_PIN_1 = 18;

// --- Konfigurasi Pin Sensor Berat #2 ---
const int DOUT_PIN_2 = 39;
const int SCK_PIN_2 = 19;

// --- Konfigurasi Pin GPS ---
const int GPS_RX_PIN = 16; // Terhubung ke TX Modul GPS
const int GPS_TX_PIN = 17; // Tidak perlu disambungkan

// --- Konfigurasi Pin Sensor Pintu ---
const int PINTU_PIN_1 = 36;
const int PINTU_PIN_2 = 34;

// Variabel Kalibrasi
float KALIBRASI_B1 = -7050.0;
float KALIBRASI_B2 = -7050.0;


void setup() {
  Serial.begin(115200);
  Serial.println("--- Babak 1 Selesai: Inisialisasi Semua Sensor ---");

  // Inisialisasi RTC (BARU)
  if (!rtc.begin()) {
    Serial.println("Modul RTC tidak ditemukan! Periksa koneksi.");
    while (1); // Hentikan program jika RTC tidak terdeteksi
  }
  
  // PENTING: Untuk mengatur waktu RTC pertama kali, hapus '//' pada baris di bawah.
  // Setelah sekali upload, kembalikan '//' dan upload lagi.
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.println("Modul RTC Siap.");

  // Inisialisasi Sensor Berat
  scale1.begin(DOUT_PIN_1, SCK_PIN_1);
  scale1.set_scale(KALIBRASI_B1);
  scale1.tare();
  Serial.println("Sensor Berat 1 Siap.");

  scale2.begin(DOUT_PIN_2, SCK_PIN_2);
  scale2.set_scale(KALIBRASI_B2);
  scale2.tare();
  Serial.println("Sensor Berat 2 Siap.");
  
  // Inisialisasi GPS
  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("Modul GPS (Serial2) Siap.");

  // Inisialisasi Sensor Pintu
  pinMode(PINTU_PIN_1, INPUT_PULLUP);
  pinMode(PINTU_PIN_2, INPUT_PULLUP);
  Serial.println("Sensor Pintu Siap.");

  Serial.println("\n--- Memulai Pembacaan ---");
}

void loop() {
  // Selalu proses data mentah dari GPS
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  // Tampilkan semua data sensor setiap 2 detik
  static unsigned long last_read_time = 0;
  if (millis() - last_read_time > 2000) { // Ubah ke 2 detik agar tidak terlalu cepat
    last_read_time = millis();

    Serial.println("------------------------------");

    // --- Cetak data Waktu (BARU) ---
    DateTime now = rtc.now();
    Serial.print("Waktu : ");
    Serial.print(now.year(), DEC); Serial.print('/');
    Serial.print(now.month(), DEC); Serial.print('/');
    Serial.print(now.day(), DEC); Serial.print(" ");
    Serial.print(now.hour(), DEC); Serial.print(':');
    Serial.print(now.minute(), DEC); Serial.print(':');
    Serial.println(now.second(), DEC);

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
    if (gps.location.isValid()) {
      Serial.print("Lat : ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(", Long : ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Lokasi GPS belum didapat (mencari sinyal...)");
    }

    // --- Cetak data Pintu ---
    bool statusPintu1 = digitalRead(PINTU_PIN_1);
    Serial.print("Pintu 1 : ");
    Serial.println(statusPintu1 == HIGH ? "Terbuka" : "Tertutup");

    bool statusPintu2 = digitalRead(PINTU_PIN_2);
    Serial.print("Pintu 2 : ");
    Serial.println(statusPintu2 == HIGH ? "Terbuka" : "Tertutup");
  }
}