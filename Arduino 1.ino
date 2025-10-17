#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial tiltSerial(2, 3); // RX=2, TX=3
LiquidCrystal_I2C lcd(0x27, 16, 2);

int redPin = 9;
int greenPin = 10;
int bluePin = 11;
int buzzerPin = 8;

int tempPin = A0;
int soilPin = A1;
int ldrPin = A2;

unsigned long lastLCDUpdate = 0;
unsigned long lastSensorUpdate = 0;
int lcdState = 0;

const unsigned long LCD_INTERVAL = 1000;     // Waktu ganti tampilan normal
const unsigned long SENSOR_INTERVAL = 300;   // Interval baca sensor

bool bahayaSuhu = false;
bool bahayaMiring = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Inisialisasi...");
  delay(1200);
  lcd.clear();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ldrPin, INPUT);

  Serial.begin(9600);
  tiltSerial.begin(9600);
  
  tiltSerial.setTimeout(150);
}

void loop() {
  unsigned long currentMillis = millis();

  // === Update sensor tiap 300 ms ===
  static float temperature = 0;
  static int soilMoisture = 0;
  static int cahaya = 0;

  if (currentMillis - lastSensorUpdate >= SENSOR_INTERVAL) {
    lastSensorUpdate = currentMillis;

    int tempValue = analogRead(tempPin);
    float voltage = tempValue * (5.0 / 1023.0);
    temperature = (voltage - 0.5) * 100.0;

    int soilValue = analogRead(soilPin);
    soilMoisture = map(soilValue, 0, 1023, 0, 117);

    int ldrValue = analogRead(ldrPin);
    cahaya = map(ldrValue, 0, 679, 0, 100);
  }

  // === Tentukan kondisi bahaya ===
  bahayaSuhu = ((temperature > 50 && soilMoisture < 30) || (cahaya > 80) || (temperature > 100 ));

  // === Logika Prioritas (tanpa delay) ===
  if (bahayaSuhu) {
    setColor(255, 0, 0);
    tone(buzzerPin, 1000);
    tampilLCD("Suhu Tinggi!", ("Kelembapan:" + String(soilMoisture) + "%").c_str());
  }
  else {
    noTone(buzzerPin);
    setColor(0, 255, 0);

    // Ganti tampilan tiap 1 detik, tapi tetap responsif
    if (currentMillis - lastLCDUpdate >= LCD_INTERVAL) {
      lastLCDUpdate = currentMillis;
      lcd.clear();

      switch (lcdState) {
        case 0:
          lcd.setCursor(0, 0);
          lcd.print("Suhu:");
          lcd.setCursor(0, 1);
          lcd.print(temperature, 1);
          lcd.print(" C");
          break;
        case 1:
          lcd.setCursor(0, 0);
          lcd.print("Kelembapan:");
          lcd.setCursor(0, 1);
          lcd.print(soilMoisture);
          lcd.print("%");
          break;
        case 2:
          lcd.setCursor(0, 0);
          lcd.print("Cahaya:");
          lcd.setCursor(0, 1);
          lcd.print(cahaya);
          lcd.print("%");
          break;
      }
      lcdState = (lcdState + 1) % 4;
    }
  }
}

// === Fungsi bantu ===
void tampilLCD(const char* baris1, const char* baris2) {
  static String lastLine1 = "";
  static String lastLine2 = "";
  // Hindari refresh terus menerus kalau teks belum berubah
  if (String(baris1) != lastLine1 || String(baris2) != lastLine2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(baris1);
    lcd.setCursor(0, 1);
    lcd.print(baris2);
    lastLine1 = baris1;
    lastLine2 = baris2;
  }
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
