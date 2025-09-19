// ================================================================
//          UJI COBA SENSOR PINTU MAGNETIK (ESP32)
// ================================================================

const int MAGNETIK_PIN_1 = 36;   // VP
const int MAGNETIK_PIN_2 = 34;   // INPUT ONLY

void setup() {
  Serial.begin(115200);

  // Pin 36 dan 34 adalah input-only -> tidak bisa pinMode(INPUT_PULLUP)
  pinMode(MAGNETIK_PIN_1, INPUT);
  pinMode(MAGNETIK_PIN_2, INPUT);

  Serial.println("Uji Sensor Magnetik ESP32");
}

void loop() {
  int status1 = digitalRead(MAGNETIK_PIN_1);
  int status2 = digitalRead(MAGNETIK_PIN_2);

  Serial.print("Magnetik 1: ");
  if (status1 == HIGH) {
    Serial.print("TERBUKA");
  } else {
    Serial.print("TERTUTUP");
  }

  Serial.print(" | Magnetik 2: ");
  if (status2 == HIGH) {
    Serial.println("TERBUKA");
  } else {
    Serial.println("TERTUTUP");
  }

  delay(500);
}
