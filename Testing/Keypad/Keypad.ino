#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Konfigurasi Keypad 4x4
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};  // Pin baris keypad
byte colPins[COLS] = {26, 25, 33, 32};  // Pin kolom keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Konfigurasi LCD I2C (alamat biasanya 0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

String inputBuffer = "";  // untuk simpan inputan dari keypad

void setup() {
  Serial.begin(115200);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Keypad + LCD");
  lcd.setCursor(0, 1);
  lcd.print("Siap Input...");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input: ");
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    Serial.print("Key Pressed: ");
    Serial.println(customKey);

    // Tambah ke buffer
    inputBuffer += customKey;

    // Tampilkan di LCD baris kedua
    lcd.setCursor(0, 1);
    lcd.print("                ");  // hapus baris dulu
    lcd.setCursor(0, 1);
    lcd.print(inputBuffer);

    // Contoh: reset input kalau tekan '#'
    if (customKey == '#') {
      inputBuffer = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
}
