#define MAGNETIC_SENSOR_PIN 34   // Input-only pin ESP32
#define MAGNETIC_SENSOR_PIN2 35   // Input-only pin ESP32
#define LED_PIN 32               // Sebagai indikator LED
#define LED_PIN2 33  

void setup() {
  pinMode(MAGNETIC_SENSOR_PIN, INPUT);
  pinMode(MAGNETIC_SENSOR_PIN2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  int sensorState = digitalRead(MAGNETIC_SENSOR_PIN);
  int sensorState2 = digitalRead(MAGNETIC_SENSOR_PIN2);

  if (sensorState == HIGH) {
    // Pintu tertutup (magnet dekat)
    digitalWrite(LED_PIN, LOW);
    Serial.println("Pintu TERTUTUP 1");
  } else {
    // Pintu terbuka (magnet menjauh)
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Pintu TERBUKA 1");
  }

    if (sensorState2 == HIGH) {
    // Pintu tertutup (magnet dekat)
    digitalWrite(LED_PIN2, LOW);
    Serial.println("Pintu TERTUTUP 2");
  } else {
    // Pintu terbuka (magnet menjauh)
    digitalWrite(LED_PIN2, HIGH);
    Serial.println("Pintu TERBUKA 2");
  }

  delay(200); // Debounce ringan
}
