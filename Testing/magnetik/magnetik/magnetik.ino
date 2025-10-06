// ================================================================
//     Uji Sensor Reed Switch Magnetik dengan GPIO 34 & 36 (ESP32)
// ================================================================

const int switchReed1 = 34;   // GPIO34 -> Sensor Reed 1
const int switchReed2 = 36;   // GPIO36 -> Sensor Reed 2

void setup() {
  Serial.begin(115200);

  // Gunakan input biasa, pull-up pakai resistor eksternal
  pinMode(switchReed1, INPUT);
  pinMode(switchReed2, INPUT);

  Serial.println("Uji Sensor Reed Switch dengan ESP32");
}

void loop() {
  int status1 = digitalRead(switchReed1);
  int status2 = digitalRead(switchReed2);

  // Sensor 1
  Serial.print("Pintu 1: ");
  if (status1 == HIGH) {
    Serial.print("TERTUTUP");
  } else {
    Serial.print("TERBUKA");
  }

  // Sensor 2
  Serial.print(" | Pintu 2: ");
  if (status2 == HIGH) {
    Serial.println("TERTUTUP");
  } else {
    Serial.println("TERBUKA");
  }

  delay(300);
}
