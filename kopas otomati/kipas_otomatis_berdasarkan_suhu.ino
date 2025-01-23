#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h> // Jangan lupa download librarynya

#define DHTPIN 9
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Alamat I2C LCD 1602
#define I2C_ADDR 0x27 // Ganti alamat ini sesuai dengan modul LCD I2C Anda

// Inisialisasi LCD
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); // 16 kolom, 2 baris

// LED dan Relay
const int blueLED = 9;
const int yellowLED = 10;
const int redLED = 11;
const int buzzerPin = 7;
const int relay1 = 6;
const int relay2 = 5;

bool isBuzzerActive = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Kipas Otomatis Berdasarkan Suhu");

  dht.begin();

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Inisialisasi LED dan Relay
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  // Pastikan relay mati saat awal
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kalibrasi
  // Sesuaikan nilai berikut berdasarkan pengukuran sebenarnya
  float suhuKalibrasi = t - 1.0; // Ganti nilai ini sesuai dengan kalibrasi

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }

  // Tampilkan hasil pembacaan suhu dan kelembaban di Serial Monitor
  Serial.print("Suhu (sebenarnya): ");
  Serial.print(t);
  Serial.print("℃, ");
  Serial.print("Suhu (terkalibrasi): ");
  Serial.print(suhuKalibrasi);
  Serial.println(" ℃");
  Serial.print("Lembap: ");
  Serial.print(h);
  Serial.println("%");

  // Tampilkan hasil pembacaan suhu dan kelembaban pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(suhuKalibrasi); // Tampilkan suhu yang terkalibrasi di LCD
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Lembap: ");
  lcd.print(h);
  lcd.print(" %");

  // Kontrol LED dan Kipas berdasarkan suhu
  if (suhuKalibrasi < 27.50) {
    digitalWrite(blueLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
    isBuzzerActive = false; // Reset status Buzzer
    digitalWrite(relay1, LOW); // Matikan Relay 1 (alirkan listrik kipas)
    digitalWrite(relay2, LOW); // Matikan Relay 2 (alirkan listrik kipas)
  } else if (suhuKalibrasi >= 27.50 && suhuKalibrasi <=   ) {
    digitalWrite(blueLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
    isBuzzerActive = false; // Reset status Buzzer
    digitalWrite(relay1, LOW); // Aktifkan Relay 1 (alirkan listrik ke kipas)
    digitalWrite(relay2, LOW); // Aktifkan Relay 2 (alirkan listrik ke kipas)
  } else {
    digitalWrite(blueLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
    
    // Aktifkan Buzzer jika suhu tinggi (hanya pertama kali)
    if (!isBuzzerActive) {
      tone(buzzerPin, 1000, 1000); // Bunyikan buzzer selama 2 detik
      digitalWrite(relay1, HIGH); // Matikan Relay 1 (matikan aliran listrik ke kipas)
      digitalWrite(relay2, HIGH); // Matikan Relay 2 (matikan aliran listrik ke kipas)
      isBuzzerActive = true; // Set status Buzzer
    }
    delay(5000); // Tunggu 5 detik sebelum mematikan Relay
  }
}
