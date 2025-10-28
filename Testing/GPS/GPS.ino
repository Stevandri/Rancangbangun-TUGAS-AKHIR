#include <TinyGPSPlus.h>

TinyGPSPlus gps;
HardwareSerial GPSSerial(2); // gunakan UART2

const int RX_PIN = 16; // koneksi ke TX modul
const int TX_PIN = 17; // koneksi ke RX modul
const uint32_t BAUD = 9600; // coba 9600; kalau modul lain pakai 115200

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("ESP32 GPS ATGM336H - TinyGPSPlus example");

  GPSSerial.begin(BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  // baca semua data yang masuk dari modul GPS
  while (GPSSerial.available()) {
  char c = GPSSerial.read();
  Serial.write(c); // tampilkan semua karakter NMEA
  gps.encode(c);
}
-

  // cek kalau ada data valid
  if (gps.location.isUpdated()) {
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print("  Lon: ");
    Serial.print(gps.location.lng(), 6);
    Serial.print("  Altitude(m): ");
    if (gps.altitude.isValid()) Serial.print(gps.altitude.meters());
    else Serial.print("n/a");
    Serial.print("  Sats: ");
    if (gps.satellites.isValid()) Serial.print(gps.satellites.value());
    else Serial.print("n/a");
    Serial.println();
  }

  // cek jumlah satelit terdeteksi
  if (gps.satellites.isUpdated()) {
    int satCount = gps.satellites.value();
    Serial.print("Jumlah Satelit Terhubung: ");
    if (satCount > 0) {
      Serial.println(satCount);
    } else {
      Serial.println("Tidak ada satelit");
    }
  }

  // contoh lainnya: waktu dari GPS
  if (gps.time.isValid()) {
    Serial.print("Time (UTC): ");
    if (gps.time.hour() < 10) Serial.print('0');
    Serial.print(gps.time.hour());
    Serial.print(':');
    if (gps.time.minute() < 10) Serial.print('0');
    Serial.print(gps.time.minute());
    Serial.print(':');
    if (gps.time.second() < 10) Serial.print('0');
    Serial.print(gps.time.second());
    Serial.print('.');
    Serial.println(gps.time.centisecond());
  }

  delay(200);
}
