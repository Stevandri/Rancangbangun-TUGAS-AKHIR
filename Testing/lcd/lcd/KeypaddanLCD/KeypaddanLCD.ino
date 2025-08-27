#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Sesuaikan alamat I2C kalau perlu

// Konfigurasi keypad 4x4
const byte ROWS = 4;
const byte COLS = 4;

// Mapping keypad (A-D diset jadi '\0' atau karakter kosong)
char keys[ROWS][COLS] = {
  {'1', '2', '3', '\0'},
  {'4', '5', '6', '\0'},
  {'7', '8', '9', '\0'},
  {'*', '0', '#', '\0'}
};

// Pin keypad ke ESP32 (ubah sesuai koneksi kamu)
byte rowPins[ROWS] = {18, 5, 17, 16}; 
byte colPins[COLS] = {4, 0, 2, 15};

// Inisialisasi keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variabel input
String inputText = "";

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ketik PIN:");
  lcd.setCursor(0, 1);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    // Abaikan tombol A-D yang dimapping ke '\0'
    if (key == '\0') return;

    if (key == '*') {
      inputText = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ketik PIN:");
      lcd.setCursor(0, 1);
    } else if (key == '#') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input selesai:");
      lcd.setCursor(0, 1);
      lcd.print(inputText);
      delay(3000);
      lcd.clear();
      inputText = "";
      lcd.setCursor(0, 0);
      lcd.print("Ketik PIN:");
      lcd.setCursor(0, 1);
    } else {
      if (inputText.length() < 16) {
        inputText += key;
        lcd.print(key);
      }
    }
  }
}
