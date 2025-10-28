#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Gunakan salah satu UART hardware ESP32 (Serial1 atau Serial2)
HardwareSerial gpsSerial(1);  // bisa 1 atau 2 tergantung pin yang kamu pilih

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  // GPS module – sambungkan ke RX2/TX2 (atau sesuai konfigurasi kamu)
  // Contoh: RX GPS ke TX2 (GPIO17), TX GPS ke RX2 (GPIO16)
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  
  Serial.println("ESP32 + NEO-6M GPS : Menampilkan Waktu & Lokasi");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid() && gps.time.isValid()) {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    int hour = gps.time.hour();
    int minute = gps.time.minute();
    int second = gps.time.second();

    // Waktu UTC yang diberikan GPS
    Serial.print("Waktu UTC: ");
    if (hour < 10) Serial.print('0');
    Serial.print(hour);
    Serial.print(':');
    if (minute < 10) Serial.print('0');
    Serial.print(minute);
    Serial.print(':');
    if (second < 10) Serial.print('0');
    Serial.println(second);

    Serial.print("Latitude: ");
    Serial.println(lat, 6);
    Serial.print("Longitude: ");
    Serial.println(lng, 6);
    Serial.println();
  } else {
    Serial.println("Menunggu data GPS valid...");
  }

  delay(1000);
}
