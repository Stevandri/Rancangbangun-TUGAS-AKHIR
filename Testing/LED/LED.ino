// Definisikan pin LED output
#define LED1 32
#define LED2 33
#define LED3 25
#define LED4 26

void setup() {
  // Set semua pin sebagai OUTPUT
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
}

void loop() {
  // Nyalakan semua LED
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED2, HIGH);
  delay(1000);
  digitalWrite(LED3, HIGH);
  delay(1000);
  digitalWrite(LED4, HIGH);
  delay(2000);

  // Matikan semua LED
  digitalWrite(LED1, LOW);
  delay(1000);
  digitalWrite(LED2, LOW);
  delay(1000);
  digitalWrite(LED3, LOW);
  delay(1000);
  digitalWrite(LED4, LOW);
  delay(1000);
}
