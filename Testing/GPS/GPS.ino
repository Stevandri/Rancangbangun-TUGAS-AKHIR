#include <TinyGPS++.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

#define RX_PIN 16  // GPS TX ke sini
#define TX_PIN 17  // GPS RX ke sini
#define GPS_BAUD 9600

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(GPS_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);

  Serial.println("Testing GPS6MV2 di ESP32 (RX=17, TX=19)...");
}

void loop() {
  while (SerialGPS.available() > 0) {
    char c = SerialGPS.read();
    Serial.write(c); // tampilkan data NMEA mentah

    gps.encode(c);

    if (gps.location.isUpdated()) {
      Serial.println("\n=== Data GPS Valid ===");
      Serial.print("Latitude  : ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude : ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      Serial.print("Altitude  : ");
      Serial.print(gps.altitude.meters());
      Serial.println(" m");
      Serial.print("Speed     : ");
      Serial.print(gps.speed.kmph());
      Serial.println(" km/h");
      Serial.println("=======================");
    }
  }
}
