// =====================================
// Kontrol 2 Relay via Serial Monitor
// Mode: Aktif LOW
// =====================================

const int relay1 = 4;  // Relay 1 di pin 4
const int relay2 = 2;  // Relay 2 di pin 2
String inputData = ""; // Menyimpan input dari Serial

void setup() {
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  // Matikan relay di awal (HIGH = nonaktif)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);

  Serial.println("=== Kontrol Relay (Aktif LOW) ===");
  Serial.println("Ketik 'buka1'  -> Aktifkan Relay 1");
  Serial.println("Ketik 'tutup1' -> Nonaktifkan Relay 1");
  Serial.println("Ketik 'buka2'  -> Aktifkan Relay 2");
  Serial.println("Ketik 'tutup2' -> Nonaktifkan Relay 2");
  Serial.println("==================================");
}

void loop() {
  // Cek apakah ada data masuk dari Serial Monitor
  if (Serial.available() > 0) {
    inputData = Serial.readStringUntil('\n');  // Baca sampai tekan Enter
    inputData.trim(); // Hapus spasi atau karakter tak perlu

    if (inputData == "buka1") {
      digitalWrite(relay1, LOW); // Aktifkan relay 1
      Serial.println("Relay 1 AKTIF ✅");
    } 
    else if (inputData == "tutup1") {
      digitalWrite(relay1, HIGH); // Nonaktifkan relay 1
      Serial.println("Relay 1 NONAKTIF ❌");
    } 
    else if (inputData == "buka2") {
      digitalWrite(relay2, LOW); // Aktifkan relay 2
      Serial.println("Relay 2 AKTIF ✅");
    } 
    else if (inputData == "tutup2") {
      digitalWrite(relay2, HIGH); // Nonaktifkan relay 2
      Serial.println("Relay 2 NONAKTIF ❌");
    } 
    else {
      Serial.println("Perintah tidak dikenal ⚠️");
    }
  }
}
