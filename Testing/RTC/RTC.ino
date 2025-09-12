#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc; // kalau pakai DS3231
// RTC_DS1307 rtc; // kalau pakai DS1307, tinggal ganti

void setup() {
  Serial.begin(9600);
  delay(3000); // tunggu serial siap

  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }

  // Jika RTC belum di-set, jalankan ini sekali:
  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, set waktu sekarang...");
    // Set waktu ke saat ini (waktu saat upload kode):
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Atau bisa set manual, misalnya:
    rtc.adjust(DateTime(2025, 9, 1, 17, 37, 0)); // YYYY,MM,DD,HH,MM,SS
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000); // update tiap detik
}
