#include <Keypad.h>

// Jumlah baris dan kolom keypad
const byte ROWS = 4; // jumlah baris
const byte COLS = 4; // jumlah kolom

// Susunan tombol keypad sesuai hardware
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Pin yang dipakai untuk baris dan kolom keypad (ubah sesuai pin ESP32 yang kamu pakai)
byte rowPins[ROWS] = {15, 2, 4, 5};     // pin untuk baris
byte colPins[COLS] = {23, 13, 12, 14};  // pin untuk kolom

// Inisialisasi keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  Serial.println("Tes Keypad ESP32");
}

void loop() {
  char key = keypad.getKey(); // ambil tombol yang ditekan

  if (key) { // kalau ada tombol ditekan
    Serial.print("Tombol ditekan: ");
    Serial.println(key);
  }
}
