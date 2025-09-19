#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "HX711.h" // For Load Cell
#include <Keypad.h> // For Keypad
#include <LiquidCrystal_I2C.h> // For LCD
#include "RTClib.h" // For RTC

// --- WiFi and Supabase Configuration ---
const char* ssid = "Stev";
const char* password = "12341234";
const char* supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
const char* supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM";
const char* lokerId = "1001"; // Ganti dengan ID unik lemari Anda

// --- ESP32 Pin Allocation as per Description ---
// Load Cell 1 (Drawer 1)
#define DOUT_PIN_1 35 // Dout pin HX711 drawer 1 
#define SCK_PIN_1 18 // Sck pin HX711 drawer 1 
HX711 scale1;
// Load Cell 2 (Drawer 2)
#define DOUT_PIN_2 39 // Dout pin HX711 drawer 2 
#define SCK_PIN_2 19 // Sck pin HX711 drawer 2 
HX711 scale2;

// Magnetic Door Sensor
const int sensorPintu1 = 36; // Pin sensor door drawer 1 [cite: 144]
const int sensorPintu2 = 34; // Pin sensor door drawer 2 [cite: 145]

// Solenoid Door with Relay
const int solenoidPin1 = 4; // Relay for drawer 1 [cite: 152]
const int solenoidPin2 = 2; // Relay for drawer 2 [cite: 153]

// Keypad
const byte ROWS = 4; // Four rows [cite: 147]
const byte COLS = 3; // Three columns [cite: 148]
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; // Row pins [cite: 147]
byte colPins[COLS] = {26, 25, 33}; // Column pins [cite: 148]
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address and screen size [cite: 149]

// RTC (DS3231)
RTC_DS3231 rtc; // RTC module [cite: 36, 150]

// --- Global Variables ---
String inputPin = "";
String kodeAmbilLaci1 = "";
String kodeAmbilLaci2 = "";
bool solenoid1Aktif = false;
bool solenoid2Aktif = false;
unsigned long waktuAktifSolenoid1 = 0;
unsigned long waktuAktifSolenoid2 = 0;

// --- Function to connect to WiFi ---
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

// --- Function to send data to Supabase ---
void sendSensorData(int laci_number, float berat, bool pintuTerbuka) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    StaticJsonDocument<200> doc;
    doc["loker_id"] = lokerId;
    doc["berat"] = berat; // Data from HX711 [cite: 46]
    doc["pintu_terbuka"] = pintuTerbuka;// Data from magnetic door sensor [cite: 47]
    doc["terakhir_update"] = "now()";

    String jsonOutput;
    serializeJson(doc, jsonOutput);

    String url = String(supabaseUrl) + "/rest/v1/sensor_status";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabaseAnonKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));
    http.addHeader("Prefer", "return=minimal");

    int httpResponseCode = http.POST(jsonOutput);
    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response Code (Send Laci %d): %d\n", laci_number, httpResponseCode);
    } else {
      Serial.printf("HTTP Error (Send Laci %d): %s\n", laci_number, http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }
}

// --- Function to get locker status from Supabase ---
void getLockerStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(supabaseUrl) + "/rest/v1/lockers?select=terkunci,titipan(kode_ambil)&id=eq." + String(lokerId);
    http.begin(url);
    http.addHeader("apikey", supabaseAnonKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseAnonKey));

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String payload = http.getString();
      StaticJsonDocument<500> doc;
      deserializeJson(doc, payload);

      if (doc.size() > 0) {
        bool isLocked = doc[0]["terkunci"]; // Get 'terkunci' status [cite: 86]
        
        // Handle code for manual access
        kodeAmbilLaci1 = doc[0]["titipan"][0]["kode_ambil"].as<String>(); 
if (doc[0]["titipan"].size() > 1) {
  kodeAmbilLaci2 = doc[0]["titipan"][1]["kode_ambil"].as<String>(); 
}

        
        if (!isLocked) { // If 'terkunci' is false, open the locker [cite: 86]
          if (!solenoid1Aktif) {
            Serial.println("Locker Unlocked by App. Activating solenoid.");
            digitalWrite(solenoidPin1, HIGH); // Activate relay for drawer 1 [cite: 87]
            solenoid1Aktif = true;
            waktuAktifSolenoid1 = millis();
          }
        }
      }
    }
    http.end();
  }
}

// --- Function to check keypad and open manually ---
void checkKeypad() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#' || inputPin.length() >= 5) { // Check PIN after pressing '#' or after 5 digits
      if (inputPin == kodeAmbilLaci1) {
        Serial.println("Correct PIN for Drawer 1. Activating solenoid.");
        digitalWrite(solenoidPin1, HIGH);
        solenoid1Aktif = true;
        waktuAktifSolenoid1 = millis();
        // Send RTC timestamp to database [cite: 123]
      } else if (inputPin == kodeAmbilLaci2) {
        Serial.println("Correct PIN for Drawer 2. Activating solenoid.");
        digitalWrite(solenoidPin2, HIGH);
        solenoid2Aktif = true;
        waktuAktifSolenoid2 = millis();
        // Send RTC timestamp to database [cite: 123]
      } else {
        Serial.println("Incorrect PIN. Try again.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Incorrect PIN");
      }
      inputPin = ""; // Reset input
    } else {
      inputPin += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter PIN: " + inputPin);
    }
  }
}

// --- Main Setup ---
void setup() {
  Serial.begin(115200);
  connectToWiFi(); // Connect to WiFi [cite: 3]

  // Initialize Sensors and Actuators
  pinMode(sensorPintu1, INPUT_PULLUP);
  pinMode(sensorPintu2, INPUT_PULLUP);
  pinMode(solenoidPin1, OUTPUT);
  pinMode(solenoidPin2, OUTPUT);
  digitalWrite(solenoidPin1, LOW); // Make sure solenoids are off
  digitalWrite(solenoidPin2, LOW);

  // Initialize Load Cells
  scale1.begin(DOUT_PIN_1, SCK_PIN_1);
  scale2.begin(DOUT_PIN_2, SCK_PIN_2);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Active");

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Could not find RTC module!");
    while (1);
  }
}

// --- Main Loop ---
void loop() {
  // Read sensor data for drawer 1
  float beratLaci1 = scale1.get_units(10);
  bool pintu1Terbuka = digitalRead(sensorPintu1);
  sendSensorData(1, beratLaci1, pintu1Terbuka);

  // Read sensor data for drawer 2
  float beratLaci2 = scale2.get_units(10);
  bool pintu2Terbuka = digitalRead(sensorPintu2);
  sendSensorData(2, beratLaci2, pintu2Terbuka);

  // Get locker status from database
  getLockerStatus();

  // Check keypad input
  checkKeypad();

  // Solenoid logic
  if (solenoid1Aktif && (millis() - waktuAktifSolenoid1 > 10000)) { // Active for 10 seconds [cite: 122]
    digitalWrite(solenoidPin1, LOW);
    solenoid1Aktif = false;
    Serial.println("Solenoid Drawer 1 deactivated.");
  }

  if (solenoid2Aktif && (millis() - waktuAktifSolenoid2 > 10000)) { // Active for 10 seconds [cite: 122]
    digitalWrite(solenoidPin2, LOW);
    solenoid2Aktif = false;
    Serial.println("Solenoid Drawer 2 deactivated.");
  }

  delay(5000); // Wait 5 seconds before next loop [cite: 13]
}