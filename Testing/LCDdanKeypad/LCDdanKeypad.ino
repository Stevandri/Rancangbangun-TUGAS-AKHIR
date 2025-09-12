#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// --- LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // kalau tidak tampil, coba ganti 0x3F

// --- Keypad 4x4 ---
const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {15, 2, 4, 5};     // pin baris
byte colPins[COLS] = {23, 13, 12, 14};  // pin kolom

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // LCD
  lcd.init();        
  lcd.backlight();   
  lcd.setCursor(0,0);
  lcd.print("Test LCD + Keypad");

  delay(2000);
  lcd.clear();
  lcd.print("Tekan Tombol:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tombol:");
    lcd.setCursor(0,1);
    lcd.print(key);
  }
}
