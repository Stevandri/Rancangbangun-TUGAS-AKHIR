#include <Wire.h>
#include <HX711.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <TinyGPSPlus.h>
#include <Arduino_JSON.h> 

TaskHandle_t SensorTask;

// ---------------- PIN MAPPING ----------------
const int LOADCELL1_DOUT = 35;
const int LOADCELL1_SCK  = 18;
const int LOADCELL2_DOUT = 39;
const int LOADCELL2_SCK  = 19;
const int MAGNET1_PIN = 36;
const int MAGNET2_PIN = 34;
const int GPS_RX_PIN = 16;
const int GPS_TX_PIN = 17;
const int RELAY1_PIN = 4;
const int RELAY2_PIN = 2;

const byte ROWS = 4;
const byte COLS = 3;
byte rowPins[ROWS] = {13, 12, 14, 27}; 
byte colPins[COLS] = {26, 25, 33};

// ---------------- OBJEK PERANGKAT ----------------
HX711 scale1;
HX711 scale2;
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
RTC_DS3231 rtc;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- VARIABEL GLOBAL ----------------
float cal1 = -10500.0;
float cal2 = -10500.0;

volatile float weight1 = 0.0;
volatile float weight2 = 0.0;
volatile bool door1_closed = false;
volatile bool door2_closed = false;
volatile float gpsLat = 0.0, gpsLng = 0.0;
volatile bool gpsValid = false;

String keypadBuffer = "";
unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL = 2000;

// ---------------- FUNGSI-FUNGSI HELPER ----------------

void handleKeypadSubmit(String code) {
  Serial.print("Keypad submit: ");
  Serial.println(code);
  
  lcd.clear();
  lcd.print("Kode Diterima!");
  delay(1500); 
  keypadBuffer = "";
  lcd.clear();
  lcd.print("Sistem Siap");
}

void setRelay(int relayPin, bool on) {
  digitalWrite(relayPin, on ? LOW : HIGH);
}

// =================================================================
// TUGAS UNTUK MEMBACA SEMUA SENSOR DI CORE 0 (BACKGROUND)
// =================================================================
void sensorTask(void *pvParameters) {
  Serial.println("Sensor Task dimulai di Core 0.");
  for (;;) {
    while (SerialGPS.available() > 0) gps.encode(SerialGPS.read());
    if (gps.location.isUpdated() && gps.location.isValid()) {
      gpsLat = gps.location.lat();
      gpsLng = gps.location.lng();
      gpsValid = true;
    }
    if (scale1.is_ready()) weight1 = scale1.get_units(5);
    if (scale2.is_ready()) weight2 = scale2.get_units(5);
    door1_closed = (digitalRead(MAGNET1_PIN) == LOW);
    door2_closed = (digitalRead(MAGNET2_PIN) == LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ---------------- SETUP (DIJALANKAN DI CORE 1) ----------------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Mulai sistem...");

  pinMode(MAGNET1_PIN, INPUT);
  pinMode(MAGNET2_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);

  Wire.begin();
  lcd.init();      
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello, world!"); 
  delay(2000);

  if (!rtc.begin()) Serial.println("ERROR: RTC tidak ditemukan!");
  else if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  scale1.begin(LOADCELL1_DOUT, LOADCELL1_SCK);
  scale1.set_scale(cal1);
  scale1.tare();
  scale2.begin(LOADCELL2_DOUT, LOADCELL2_SCK);
  scale2.set_scale(cal2);
  scale2.tare();
  
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  xTaskCreatePinnedToCore(sensorTask, "SensorTask", 10000, NULL, 1, &SensorTask, 0);           

  lcd.clear();
  lcd.print("Sistem Siap");
}

// ---------------- LOOP UTAMA (DIJALANKAN DI CORE 1) ----------------
void loop() {
  char k = keypad.getKey();
  if (k) { 
    if (k == '*') {
      keypadBuffer = "";
    } else if (k == '#') {
      handleKeypadSubmit(keypadBuffer);
    } else {
      keypadBuffer += k;
    }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Input Kode:");
    
    // Angka akan muncul di baris kedua LCD
    lcd.setCursor(0,1);
    lcd.print(keypadBuffer);
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "buka1")       setRelay(RELAY1_PIN, true);
    else if (cmd == "tutup1") setRelay(RELAY1_PIN, false);
    else if (cmd == "buka2")  setRelay(RELAY2_PIN, true);
    else if (cmd == "tutup2") setRelay(RELAY2_PIN, false);
    else if (cmd == "tare1")  scale1.tare();
    else if (cmd == "tare2")  scale2.tare();
    else if (cmd.startsWith("setcal1 ")) {
      cal1 = cmd.substring(8).toFloat();
      scale1.set_scale(cal1);
    }
    else if (cmd.startsWith("setcal2 ")) {
      cal2 = cmd.substring(8).toFloat();
      scale2.set_scale(cal2);
    }
  }

  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();
    DateTime t = rtc.now();
    char timeBuf[32];
    sprintf(timeBuf, "%04d-%02d-%02d %02d:%02d:%02d", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());

    Serial.println("\n---- STATUS ----");
    Serial.print("Waktu RTC   : "); Serial.println(timeBuf);
    Serial.print("Berat 1     : "); Serial.print(weight1, 3); Serial.println(" kg");
    Serial.print("Berat 2     : "); Serial.print(weight2, 3); Serial.println(" kg");
    Serial.print("Pintu 1     : ");
    Serial.println(door1_closed ? "Terbuka" : "Tertutup");
    Serial.print("Pintu 2     : "); Serial.println(door2_closed ? "Terbuka" : "Tertutup");
    Serial.print("GPS         : ");
    if (gpsValid) {
      Serial.print(gpsLat, 6); Serial.print(", "); Serial.println(gpsLng, 6);
    } else {
      Serial.println("Mencari sinyal...");
    }
    Serial.println("----------------");
  }
}