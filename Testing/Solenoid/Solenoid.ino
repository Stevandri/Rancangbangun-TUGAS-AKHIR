// Pin relay
#define RELAY1 4   // Relay channel 1 (solenoid door lock)

String inputData = "";

void setup() {
  // Set pin relay sebagai output
  pinMode(RELAY1, OUTPUT);

  // Relay biasanya aktif LOW (tergantung modul), kita matikan dulu
  digitalWrite(RELAY1, HIGH);

  // Serial monitor
  Serial.begin(115200);
  Serial.println("=== Kontrol Solenoid Door Lock ===");
  Serial.println("Ketik 'buka'  -> Solenoid terbuka");
  Serial.println("Ketik 'tutup' -> Solenoid terkunci");
}

void loop() {
  // Cek apakah ada data dari Serial Monitor
  if (Serial.available() > 0) {
    inputData = Serial.readStringUntil('\n'); // Baca input sampai Enter ditekan
    inputData.trim(); // hapus spasi/enter berlebih

    if (inputData.equalsIgnoreCase("buka")) {
      digitalWrite(RELAY1, LOW);   // Aktifkan relay (solenoid terbuka)
      Serial.println("🔓 Solenoid terbuka!");
    } 
    else if (inputData.equalsIgnoreCase("tutup")) {
      digitalWrite(RELAY1, HIGH);  // Matikan relay (solenoid tertutup)
      Serial.println("🔒 Solenoid tertutup!");
    } 
    else {
      Serial.println("⚠️ Perintah tidak dikenali. Ketik 'buka' atau 'tutup'.");
    }
  }
}
