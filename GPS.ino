#define rLed 6
#define gLed 7

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

long lastRecord = 0;
int interval = 1000;

String l1 = "";
String l2 = "";
String l3 = "";
String d = "";
String t = "";

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

File file;

void setup()
{

  pinMode(rLed, OUTPUT);
  pinMode(gLed, OUTPUT);

  Serial.begin(115200);
  ss.begin(GPSBaud);
  ss.setTimeout(1);

  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();

  Serial.println(F("Init SD..."));

  if (!SD.begin(5)) {
    Serial.println(F("Init SD Failed!"));
    blinkRed();
    return;
  } else {
    blinkGreen();
  }

  Serial.println(F("Init SD Success!"));

  SD.remove("log.txt");
  blinkGreen();

  Serial.println(F("Creating file!"));

  file = SD.open("log.txt", FILE_WRITE);
  blinkGreen();
  file.close();
  blinkGreen();

  Serial.println(F("File created!"));
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.

  Read();

  if (millis() > lastRecord + interval) {

    if (gps.location.isValid())
    {
      l1 = String(gps.location.lat(), 6);
      l2 = String(gps.location.lng(), 6);
      l3 = l1 + "," + l2;
    }

    Serial.println("L3: " + l3);

    if (gps.date.isValid())
    {
      d = String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year());
    }

    Serial.println("d: " + d);

    if (gps.time.isValid())
    {
      t = " " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
    }

    Serial.println("t: " + t);

    writeLCD(l1, l2);

    if (l3 != "") {
      writeFile(l3, d + t);
    }

    lastRecord = millis();
  }
}

void writeLCD(String l1, String l2) {
  lcd.setCursor(0, 0);
  lcd.print("Lat: " + l1);
  lcd.setCursor(0, 1);
  lcd.print("Lon: " + l2);
}

void writeFile(String latlon, String datetime) {
  if (SD.exists("log.txt")) {
    //blinkGreen();
    file = SD.open("log.txt", FILE_WRITE);
    //blinkGreen();

    if (file) {
      file.println(latlon + " " + datetime);
      file.close();
      Serial.println("Written!");
      blinkGreen();
    } else {
      Serial.println("File not accessible!");
      blinkRed();
    }
  } else {
    Serial.println("File doesn't exist!");
    blinkRed();
  }
}

void Read() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

void blinkRed() {
  digitalWrite(rLed, HIGH);
  delay(5);
  digitalWrite(rLed, LOW);
}

void blinkGreen() {
  digitalWrite(gLed, HIGH);
  delay(5);
  digitalWrite(gLed, LOW);
}
