// =================================================================
//        SKETCH TERPADU DENGAN SUPABASE VIA HTTPCLIENT
// =================================================================

// ---------------- LIBRARY ----------------
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <HX711.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <TinyGPSPlus.h>
#include <Arduino_JSON.h>

// ---------------- KONFIGURASI KONEKSI (WAJIB DIISI) ----------------
const char* WIFI_SSID = "Stev";
const char* WIFI_PASSWORD = "12341234";

String supabaseUrl = "https://swixpgqbndtgiyysaqrn.supabase.co";
String supabaseAnonKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InN3aXhwZ3FibmR0Z2l5eXNhcXJuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NTIzNTExMjksImV4cCI6MjA2NzkyNzEyOX0.DiPalpI_oFtPWVuvb7DK5XZyKl7mj7yKrhwbJycMakM"; // Ganti dengan Anon Key Supabase Anda

// ---------------- ID LACI DI DATABASE (WAJIB DIISI) ----------------
String laci1Id = "299a21dc-afac-4078-816c-2454adfb34f2";
String laci2Id = "a9bcb51c-c979-4417-952b-871ce500071b";

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
//---
const int LED_HIJAU_1 = 15;
const int LED_MERAH_1 = 32;
const int LED_HIJAU_2 = 23;
const int LED_MERAH_2 = 5;

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
char keys[ROWS][COLS] = {{'1','2','3'}, {'4','5','6'}, {'7','8','9'}, {'*','0','#'}}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- VARIABEL GLOBAL ----------------
float cal1 = -44938.971;
float cal2 = -104.586;
volatile float weight1 = 0.0, weight2 = 0.0;
volatile bool door1_closed = false, door2_closed = false;
volatile float gpsLat = 0.0, gpsLng = 0.0;
volatile bool gpsValid = false;
String keypadBuffer = "";

// Variabel untuk menyimpan kode ambil yang aktif
String activeCodeLaci1 = "";
String activeCodeLaci2 = "";

unsigned long lastSerialPrint = 0, lastSupabaseUpdate = 0, lastCodeFetch = 0;
unsigned long lastRemoteCheck = 0;
const unsigned long SERIAL_PRINT_INTERVAL = 2000;
const unsigned long SUPABASE_UPDATE_INTERVAL = 7000;
const unsigned long CODE_FETCH_INTERVAL = 60000;
const unsigned long REMOTE_CHECK_INTERVAL = 3000;


// --- FUNGSI-FUNGSI ---
// [FITUR BARU] Cek perintah buka laci dari database
void checkRemoteUnlock() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  
  // Perbarui query untuk mengambil kolom 'status' dari tabel 'laci'
  String url = supabaseUrl + "/rest/v1/laci?select=id,terkunci,status";
  
  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String payload = http.getString();
    JSONVar jsonPayload = JSON.parse(payload);

    if (JSON.typeof(jsonPayload) == "array") {
      for (int i = 0; i < jsonPayload.length(); i++) {
        String laciId = (const char*) jsonPayload[i]["id"];
        bool isLocked = (bool) jsonPayload[i]["terkunci"];
        String statusLaci = (const char*) jsonPayload[i]["status"]; // Ambil status laci

        // --- [TAMBAHKAN INI] Logika untuk Kontrol LED ---
        if (laciId == laci1Id) {
          if (statusLaci == "terisi") {
            digitalWrite(LED_MERAH_1, HIGH);
            digitalWrite(LED_HIJAU_1, LOW);
          } else { // Asumsi status lainnya adalah "kosong"
            digitalWrite(LED_MERAH_1, LOW);
            digitalWrite(LED_HIJAU_1, HIGH);
          }
        } else if (laciId == laci2Id) {
          if (statusLaci == "terisi") {
            digitalWrite(LED_MERAH_2, HIGH);
            digitalWrite(LED_HIJAU_2, LOW);
          } else { // Asumsi status lainnya adalah "kosong"
            digitalWrite(LED_MERAH_2, LOW);
            digitalWrite(LED_HIJAU_2, HIGH);
          }
        }
        // ------------------------------------------------

        // Logika remote unlock (tidak berubah)
        if (!isLocked) {
          if (laciId == laci1Id) digitalWrite(RELAY1_PIN, LOW);
          else if (laciId == laci2Id) digitalWrite(RELAY2_PIN, LOW);
        } else {
          if (laciId == laci1Id) digitalWrite(RELAY1_PIN, HIGH);
          else if (laciId == laci2Id) digitalWrite(RELAY2_PIN, HIGH);
        }
      }
    }
  }
  http.end();
}


// [LOGIKA KODE AMBIL] Fungsi yang dijalankan setelah kode benar
void processCorrectCode(String code, int relayPin) {
  digitalWrite(relayPin, LOW); // Buka kunci
  lcd.clear();
  lcd.print("Laci Terbuka!");

  DateTime now = rtc.now();
  char now_iso[25];
  sprintf(now_iso, "%04d-%02d-%02dT%02d:%02d:%02dZ", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  String titipanPayload = "{\"waktu_diambil\":\"" + String(now_iso) + "\", \"status\":\"diambil\"}";
  
  HTTPClient http;
  String titipanUrl = supabaseUrl + "/rest/v1/titipan?kode_ambil=eq." + code;
  http.begin(titipanUrl);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);
  http.addHeader("Content-Type", "application/json");
  http.PATCH(titipanPayload);
  http.end();

  String laciIdToUpdate = (relayPin == RELAY1_PIN) ? laci1Id : laci2Id;
  String laciPayload = "{\"status\":\"kosong\"}";
  patchToSupabase("laci", laciIdToUpdate, laciPayload);

  if (relayPin == RELAY1_PIN) {
    activeCodeLaci1 = "";
    Serial.println("Variabel activeCodeLaci1 dikosongkan.");
  } else if (relayPin == RELAY2_PIN) {
    activeCodeLaci2 = "";
    Serial.println("Variabel activeCodeLaci2 dikosongkan.");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Tutup Kembali");

  int magnetPin = (relayPin == RELAY1_PIN) ? MAGNET1_PIN : MAGNET2_PIN;
  while (digitalRead(magnetPin) != LOW) {
    delay(100);
  }

  digitalWrite(relayPin, HIGH); // Kunci kembali
  lcd.clear();
  lcd.print("Masukkan Kode:");
}

// [SUPABASE] Fungsi untuk mengirim permintaan PATCH
void patchToSupabase(String table, String id, String jsonPayload) {
  if (WiFi.status() != WL_CONNECTED) { return; }
  HTTPClient http;
  String url = supabaseUrl + "/rest/v1/" + table + "?id=eq." + id;
  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Prefer", "return=minimal");
  http.PATCH(jsonPayload);
  http.end();
}

// [SUPABASE] Fungsi untuk mengirim semua data sensor
void sendSensorDataToSupabase() {
  String laci1Payload = "{\"berat_titipan\":" + String(weight1, 2) + ", \"magnetik_door\":" + (door1_closed ? "true" : "false") + "}";
  patchToSupabase("laci", laci1Id, laci1Payload);
  String laci2Payload = "{\"berat_titipan\":" + String(weight2, 2) + ", \"magnetik_door\":" + (door2_closed ? "true" : "false") + "}";
  patchToSupabase("laci", laci2Id, laci2Payload);
  if (gpsValid) {
    String gpsPayload = "{\"koordinat_lat\":" + String(gpsLat, 6) + ", \"koordinat_long\":" + String(gpsLng, 6) + "}";
    patchToSupabase("GPS", "1", gpsPayload);
  }
}

// [LOGIKA KODE AMBIL] Fungsi untuk mengambil kode aktif dari Supabase
void fetchActiveCodes() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = supabaseUrl + "/rest/v1/titipan?select=kode_ambil,laci_id&status=eq.dititipkan";
  
  http.begin(url);
  http.addHeader("apikey", supabaseAnonKey);
  http.addHeader("Authorization", "Bearer " + supabaseAnonKey);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    Serial.println("Response kode aktif: " + payload);

    // Parse string JSON menjadi objek yang bisa dibaca
    JSONVar jsonPayload = JSON.parse(payload);

    // Periksa apakah parsing berhasil dan hasilnya adalah Array
    if (JSON.typeof(jsonPayload) == "array") {
      // Reset kode sebelum diisi ulang
      activeCodeLaci1 = "";
      activeCodeLaci2 = "";

      // Loop melalui setiap item di dalam array JSON
      for (int i = 0; i < jsonPayload.length(); i++) {
        String laciIdFromJson = (const char*) jsonPayload[i]["laci_id"];
        String kodeAmbilFromJson = (const char*) jsonPayload[i]["kode_ambil"];

        // Cocokkan laci_id dan simpan kodenya
        if (laciIdFromJson == laci1Id) {
          activeCodeLaci1 = kodeAmbilFromJson;
        } else if (laciIdFromJson == laci2Id) {
          activeCodeLaci2 = kodeAmbilFromJson;
        }
      }
    } else {
      Serial.println("Gagal parsing JSON, format tidak sesuai array.");
    }
    
    Serial.println("Kode Laci 1 tersimpan: " + activeCodeLaci1);
    Serial.println("Kode Laci 2 tersimpan: " + activeCodeLaci2);

  } else {
    Serial.printf("Gagal ambil kode, HTTP: %d\n", httpResponseCode);
  }
  http.end();
}

// Tugas untuk membaca semua sensor di Core 0
void sensorTask(void *pvParameters) {
  Serial.println("Sensor Task dimulai di Core 0.");
  for (;;) {
    while (SerialGPS.available() > 0) gps.encode(SerialGPS.read());
    if (gps.location.isUpdated() && gps.location.isValid()) {
      gpsLat = gps.location.lat();
      gpsLng = gps.location.lng();
      gpsValid = true;
    }
    if (scale1.is_ready()) weight1 = scale1.get_units(10);
    if (scale2.is_ready()) weight2 = scale2.get_units(10);
    door1_closed = (digitalRead(MAGNET1_PIN) == LOW);
    door2_closed = (digitalRead(MAGNET2_PIN) == LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  pinMode(MAGNET1_PIN, INPUT_PULLUP);
  pinMode(MAGNET2_PIN, INPUT_PULLUP);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  //--
  pinMode(LED_HIJAU_1, OUTPUT);
  pinMode(LED_MERAH_1, OUTPUT);
  pinMode(LED_HIJAU_2, OUTPUT);
  pinMode(LED_MERAH_2, OUTPUT);
  digitalWrite(LED_HIJAU_1, LOW);
  digitalWrite(LED_MERAH_1, LOW);
  digitalWrite(LED_HIJAU_2, LOW);
  digitalWrite(LED_MERAH_2, LOW);
  Wire.begin();
  
  lcd.init();      
  lcd.backlight();
  lcd.clear();
  lcd.print("Hubungkan WiFi..");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print("."); lcd.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  lcd.clear(); lcd.print("WiFi Terhubung!");
  delay(1500);

  if (!rtc.begin()) Serial.println("ERROR: RTC tidak ditemukan!");
  else if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  scale1.begin(LOADCELL1_DOUT, LOADCELL1_SCK);
  scale1.set_scale(cal1);
  scale1.tare();
  scale2.begin(LOADCELL2_DOUT, LOADCELL2_SCK);
  scale2.set_scale(cal2);
  scale2.tare();
  
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  xTaskCreatePinnedToCore(sensorTask, "SensorTask", 10000, NULL, 1, NULL, 0);           

  fetchActiveCodes();

  lcd.clear();
  lcd.print("Masukkan Kode:");
}

// ---------------- LOOP UTAMA ----------------
void loop() {
  char k = keypad.getKey();
  if (k) { 
    if (k == '*') {
      keypadBuffer = "";
    } else if (k == '#') {
      if (keypadBuffer == activeCodeLaci1 && activeCodeLaci1 != "") {
        processCorrectCode(keypadBuffer, RELAY1_PIN);
      } else if (keypadBuffer == activeCodeLaci2 && activeCodeLaci2 != "") {
        processCorrectCode(keypadBuffer, RELAY2_PIN);
      } else {
        lcd.clear(); 
        lcd.print("Kode Salah!");
        delay(1500);
      }
      keypadBuffer = "";
    } else { 
      keypadBuffer += k;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kode Ambil:");
    lcd.setCursor(0, 1);
    lcd.print(keypadBuffer);
  }

  unsigned long now = millis();

  if (now - lastRemoteCheck >= REMOTE_CHECK_INTERVAL) {
    lastRemoteCheck = now;
    checkRemoteUnlock();
  }

  if (now - lastCodeFetch >= CODE_FETCH_INTERVAL) {
    lastCodeFetch = now;
    fetchActiveCodes();
  }
  if (now - lastSupabaseUpdate >= SUPABASE_UPDATE_INTERVAL) {
    lastSupabaseUpdate = now;
    sendSensorDataToSupabase();
  }
  
  if (now - lastSerialPrint >= SERIAL_PRINT_INTERVAL) {
    lastSerialPrint = now;
    
    DateTime t = rtc.now();
    char timeBuf[32];
    sprintf(timeBuf, "%04d-%02d-%02d %02d:%02d:%02d", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());

    Serial.println("\n---- STATUS SENSOR ----");
    Serial.print("Waktu RTC   : "); Serial.println(timeBuf);
    Serial.print("Berat 1     : "); Serial.print(weight1, 3); Serial.println(" kg");
    Serial.print("Berat 2     : "); Serial.print(weight2, 3); Serial.println(" kg");
    Serial.print("Pintu 1     : "); Serial.println(door1_closed ? "Tertutup" : "Terbuka");
    Serial.print("Pintu 2     : "); Serial.println(door2_closed ? "Tertutup" : "Terbuka");
    Serial.print("GPS         : ");
    if (gpsValid) {
      Serial.print(gpsLat, 6); Serial.print(", "); Serial.println(gpsLng, 6);
    } else {
      Serial.println("Mencari sinyal...");
    }
    Serial.println("-----------------------");
  }
}