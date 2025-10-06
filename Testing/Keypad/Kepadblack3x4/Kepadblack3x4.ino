#include <Keypad.h>

// Jumlah baris dan kolom keypad
const byte ROWS = 4; 
const byte COLS = 3; 

// Mapping tombol keypad 4x3
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}                                                            
};

// Definisi pin baris (R1-R4) dan kolom (C1-C3)
byte rowPins[ROWS] = {13, 12, 14, 27}; 
byte colPins[COLS] = {26, 25, 33};    

// Inisialisasi keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(){
  Serial.begin(115200);
  Serial.println("Keypad ESP32 Ready...");
}

void loop(){
  char key = keypad.getKey();

  if (key) { // Jika tombol ditekan
    Serial.print("Tombol ditekan: ");
    Serial.println(key);
  }
}
