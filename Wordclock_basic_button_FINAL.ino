#include <DS3232RTC.h> //https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h> //https://github.com/PaulStoffregen/Time
#include <Wire.h>
#include <FastLED.h> //https://github.com/FastLED/FastLED
/*
   Wordclock for WS2812B LED Pixel Stripes and 3231 RTC
   More Information at https://dracarysdiy.wordpress.com
   and https://github.com/FastLED/FastLED/wiki/Overview
*/
FASTLED_USING_NAMESPACE
#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
//#define LED_TYPE    APA102
//#define COLOR_ORDER BGR
#define NUM_LEDS    99  // Gesamtanzahl der WS2812 LEDs
CRGB leds[NUM_LEDS];

uint8_t maxBrightness     = 35;
// Wenn Fotoresistor benutzt wird, hier
//min. Helligkeit eintragen (0=Off->255=max)
uint8_t minBrightness     = 210;
// Wenn Fotoresistor benutzt wird, hier
//max. Helligkeit eintragen (0=Off->255=max)
uint8_t AmbientLightPin   = 0;
// Fotoresistor Pin
uint8_t BRIGHTNESS        = 120;
// Wenn kein Fotoresistor benutzt wird
//hier dauerhafte Helligkeit eintragen

uint8_t Stunde     = 0;
uint8_t Std        = 12;
uint8_t Minute     = 15;
uint8_t WochenTag  = 0;
uint8_t Tag        = 30;
uint8_t Monat      = 10;
boolean DST        = false;

//button
const byte buttonPin = 7;    // momentary push button on pin 7
int bH = 11;
int bM = 10;
  int stateH = 0;
  int stateM = 0;
bool oldButtonVal = 0;
int nPatterns = 2;
int lightPattern = 2; //Start with this light
//End button

//***HIER LED POSITIONEN EINTRAGEN***//

int EsIst[]      = {65};
int Eins[]      = {47,48,49,50};
int Ein[]       = {47,48,49};
int Zwei[]      = {84,85,86,87};
int Drei[]      = {45,46,47,48};//
int Vier[]      = {89,90,91,92};
int Fuenf[]     = {33,34,35,36};
int Sechs[]     = {50,51,52,53,54};
int Sieben[]    = {67,68,69,70,71,72};
int Acht[]      = {95,96,97,98};
int Neun[]      = {72,73,74,75};
int Zehn[]      = {59,60,61,62};
int Elf[]       = {36,37,38};
int Zwoelf[]    = {79,80,81,82,83};
int MZwanzig[]  = {6,7,8,9,18,19,20,40,41,42,43};
int MFuenf[]    = {1,2,3,4};
int MZehn[]     = {6,7,8,9};

int Viertel[]   = {26,27,28,29,30,31,32};
int Vor[]       = {18,19,20};
int Nach[]      = {13,14,15,16};
int Halb[]      = {40,41,42,43,43};
int Uhr[]       = {65};
int Und[]       = {65};

int EinsM[]     = {65}; //Minuten Punkt 1
int ZweiM[]     = {65}; //Minuten Punkt 2
int DreiM[]     = {65}; //Minuten Punkt 3
int VierM[]     = {65}; //Minuten Punkt 4
//**********************************//

int i, Ambient, LastAmbient;
CRGBPalette16 gPal;

void setup() {
  delay(3000); // Sicherheits Delay
  Serial.begin(19200);
  Serial.println("Setup Begin ...");

  //button
  pinMode(buttonPin, INPUT_PULLUP);  // button pin is HIGH, so it drops to 0 if pressed
  pinMode(bH, INPUT_PULLUP);
  pinMode(bM, INPUT_PULLUP);

  //End button

  //LEDs werden eingefügt + Farbkorrektur
  //und Farbtemperatur auf Wolfram (warmweiß)
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();  // clear all pixel data
  FastLED.show();
  //FastLED.addLeds<LED_TYPE, COLOR_ORDER>(leds, NUM_LEDS);
  Serial.println("FastLed...");
  FastLED.setTemperature( Tungsten40W );
  gPal = Rainbow_gp;
  Serial.println("Starte Wordclock ...");
  
  //Versuche auf RTC zu zugreifen
  
  setSyncProvider(DS3232RTC::get);
  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else if (timeStatus() != timeNeedsSync) {
    Serial.println("RTC has set the system time");
  } else {
    Serial.println("RTC has set the system time but needs Sync");
  }
  
}
const CRGB Rainbow[] =
{
    0xFF0000, 0xD52A00, 0xAB5500, 0xAB7F00,
    0xABAB00, 0x56D500, 0x00FF00, 0x00D52A,
    0x00AB55, 0x0056AA, 0x0000FF, 0x2A00D5,
    0x5500AB, 0x7F0081, 0xAB0055, 0xD5002B
}; 
void SwitchLED(int MyArray[], int n) {
  static byte colorindex;
  //Umgebungshelligkeit überprüfen (sofern gewünscht)
  if (maxBrightness != 0) {
    Ambient = analogRead(AmbientLightPin);
    //Nur bei größeren Helligkeitsänderungen
    //soll die Helligkeit der LEDs angepasst werden:
    if ((Ambient > LastAmbient * 1.10) || (Ambient < LastAmbient * 0.90)) {BRIGHTNESS = map(Ambient, 0, 1023, maxBrightness, minBrightness);
      //BRIGHTNESS = 25;
      LastAmbient = Ambient;
    }
  }
  //Funktion zum Anschalten der LEDs in warmweiß
  for (i = MyArray[0]; i < MyArray[0] + n; i++) {
    leds[i] = Rainbow[colorindex];
    //leds[i].setRGB(255, 0, 0); // Red
    // HEX Warmweiß | Hier gewünschte LED Farbe (HEX) eintragen
    //pattern1();
    FastLED.setBrightness(BRIGHTNESS);
    //leds[i] = CHSV(140, 27, BRIGHTNESS);
    // ODER hier gewünschte Farbe in HSV
    //(  CHSV(FARBE, SÄTTIGUNG, BRIGHTNESS)  )
    //leds[i].setRGB(255, 68, 221);
    // ODER hier gewünschte Farbe in RGB
    //(  .setRGB(Rot,Grün,Blau)   )
  }
  bool buttonVal = digitalRead(buttonPin);
  if (!buttonVal && oldButtonVal) {
    // button has just been pressed
    colorindex = (colorindex + 1) % 16;
    Serial.print("colorindex ");
    Serial.println(colorindex);
  }
  oldButtonVal = buttonVal;
}

void displaytime(void) {
  //zuerst setzten wir alle LEDs zurück
  fill_solid( leds, NUM_LEDS, CHSV(0, 0, 0));

  // Nun suchen wir die richtigen LEDs
  //und übergeben sie an die Funktion zum Anschalten
  Serial.print("Es ist ");
  SwitchLED(EsIst, (sizeof(EsIst) / 2));

  if (((Minute > 4) && (Minute < 10)) || (Minute > 54)) {
    SwitchLED(MFuenf, (sizeof(MFuenf) / 2));
    Serial.print("5 Minuten ");
  }
  if (((Minute > 9) && (Minute < 15))
      || ((Minute > 49) && (Minute < 55))) {
    SwitchLED(MZehn, (sizeof(MZehn) / 2));
    Serial.print("10 Minuten ");
  }
  if (((Minute > 14) && (Minute < 20))
      || ((Minute > 44) && (Minute < 50))) {
    SwitchLED(Viertel, (sizeof(Viertel) / 2));
    Serial.print("Viertel ");
  }
  if (((Minute > 19) && (Minute < 25))
      || ((Minute > 39) && (Minute < 45))) {
    SwitchLED(MZwanzig, (sizeof(MZwanzig) / 2));
    Serial.print("20 Minuten ");
  }
  if (((Minute > 24) && (Minute < 30))
      || ((Minute > 34) && (Minute < 40))) {
    SwitchLED(MFuenf, (sizeof(MFuenf) / 2));
    SwitchLED(Und, (sizeof(Und) / 2));
    SwitchLED(MZwanzig, (sizeof(MZwanzig) / 2));
    Serial.print("25 Minuten ");
  }
  if ((Minute > 29) && (Minute < 35)) {
    SwitchLED(Halb, (sizeof(Halb) / 2));
    Serial.print("Halb ");
  }

  if (Minute < 5) {
    switch (Stunde) {
      case 1:
        SwitchLED(Ein, (sizeof(Ein) / 2));
        Serial.print("1 ");
        break;
      case 2:
        SwitchLED(Zwei, (sizeof(Zwei) / 2));
        Serial.print("2 ");
        break;
      case 3:
        SwitchLED(Drei, (sizeof(Drei) / 2));
        Serial.print("3 ");
        break;
      case 4:
        SwitchLED(Vier, (sizeof(Vier) / 2));
        Serial.print("4 ");
        break;
      case 5:
        SwitchLED(Fuenf, (sizeof(Fuenf) / 2));
        Serial.print("5 ");
        break;
      case 6:
        SwitchLED(Sechs, (sizeof(Sechs) / 2));
        Serial.print("6 ");
        break;
      case 7:
        SwitchLED(Sieben, (sizeof(Sieben) / 2));
        Serial.print("7 ");
        break;
      case 8:
        SwitchLED(Acht, (sizeof(Acht) / 2));
        Serial.print("8 ");
        break;
      case 9:
        SwitchLED(Neun, (sizeof(Neun) / 2));
        Serial.print("9 ");
        break;
      case 10:
        SwitchLED(Zehn, (sizeof(Zehn) / 2));
        Serial.print("10 ");
        break;
      case 11:
        SwitchLED(Elf, (sizeof(Elf) / 2));
        Serial.print("11 ");
        break;
      case 12:
        SwitchLED(Zwoelf, (sizeof(Zwoelf) / 2));
        Serial.print("12 ");
        break;
    }
    SwitchLED(Uhr, (sizeof(Uhr) / 2));
    Serial.println("Uhr");
  }
  else if ((Minute < 30) && (Minute > 4)) {
    SwitchLED(Nach, (sizeof(Nach) / 2));
    Serial.print("nach ");
    switch (Stunde) {
      case 1:
        SwitchLED(Eins, (sizeof(Eins) / 2));
        Serial.println("1 ");
        break;
      case 2:
        SwitchLED(Zwei, (sizeof(Zwei) / 2));
        Serial.println("2 ");
        break;
      case 3:
        SwitchLED(Drei, (sizeof(Drei) / 2));
        Serial.println("3 ");
        break;
      case 4:
        SwitchLED(Vier, (sizeof(Vier) / 2));
        Serial.println("4 ");
        break;
      case 5:
        SwitchLED(Fuenf, (sizeof(Fuenf) / 2));
        Serial.println("8 ");
        break;
      case 6:
        SwitchLED(Sechs, (sizeof(Sechs) / 2));
        Serial.println("6 ");
        break;
      case 7:
        SwitchLED(Sieben, (sizeof(Sieben) / 2));
        Serial.println("7 ");
        break;
      case 8:
        SwitchLED(Acht, (sizeof(Acht) / 2));
        Serial.println("8 ");
        break;
      case 9:
        SwitchLED(Neun, (sizeof(Neun) / 2));
        Serial.println("9 ");
        break;
      case 10:
        SwitchLED(Zehn, (sizeof(Zehn) / 2));
        Serial.println("10 ");
        break;
      case 11:
        SwitchLED(Elf, (sizeof(Elf) / 2));
        Serial.println("11 ");
        break;
      case 12:
        SwitchLED(Zwoelf, (sizeof(Zwoelf) / 2));
        Serial.println("12 ");
        break;
    }
  }
  else {
    if ((Minute > 29) && (Minute < 35)) {
      switch (Stunde) {
        case 1:
          SwitchLED(Zwei, (sizeof(Zwei) / 2));
          Serial.println("2 ");
          break;
        case 2:
          SwitchLED(Drei, (sizeof(Drei) / 2));
          Serial.println("3");
          break;
        case 3:
          SwitchLED(Vier, (sizeof(Vier) / 2));
          Serial.println("4 ");
          break;
        case 4:
          SwitchLED(Fuenf, (sizeof(Fuenf) / 2));
          Serial.println("5 ");
          break;
        case 5:
          SwitchLED(Sechs, (sizeof(Sechs) / 2));
          Serial.println("6 ");
          break;
        case 6:
          SwitchLED(Sieben, (sizeof(Sieben) / 2));
          Serial.println("7 ");
          break;
        case 7:
          SwitchLED(Acht, (sizeof(Acht) / 2));
          Serial.println("8 ");
          break;
        case 8:
          SwitchLED(Neun, (sizeof(Neun) / 2));
          Serial.println("9 ");
          break;
        case 9:
          SwitchLED(Zehn, (sizeof(Zehn) / 2));
          Serial.println("10 ");
          break;
        case 10:
          SwitchLED(Elf, (sizeof(Elf) / 2));
          Serial.println("11 ");
          break;
        case 11:
          SwitchLED(Zwoelf, (sizeof(Zwoelf) / 2));
          Serial.println("12 ");
          break;
        case 12:
          SwitchLED(Eins, (sizeof(Eins) / 2));
          Serial.println("1 ");
          break;
      }
    }
    else {
      SwitchLED(Vor, (sizeof(Vor) / 2));
      Serial.print("vor ");
      switch (Stunde) {
        case 1:
          SwitchLED(Zwei, (sizeof(Zwei) / 2));
          Serial.println("2 ");
          break;
        case 2:
          SwitchLED(Drei, (sizeof(Drei) / 2));
          Serial.println("3");
          break;
        case 3:
          SwitchLED(Vier, (sizeof(Vier) / 2));
          Serial.println("4 ");
          break;
        case 4:
          SwitchLED(Fuenf, (sizeof(Fuenf) / 2));
          Serial.println("5 ");
          break;
        case 5:
          SwitchLED(Sechs, (sizeof(Sechs) / 2));
          Serial.println("6 ");
          break;
        case 6:
          SwitchLED(Sieben, (sizeof(Sieben) / 2));
          Serial.println("7 ");
          break;
        case 7:
          SwitchLED(Acht, (sizeof(Acht) / 2));
          Serial.println("8 ");
          break;
        case 8:
          SwitchLED(Neun, (sizeof(Neun) / 2));
          Serial.println("9 ");
          break;
        case 9:
          SwitchLED(Zehn, (sizeof(Zehn) / 2));
          Serial.println("10 ");
          break;
        case 10:
          SwitchLED(Elf, (sizeof(Elf) / 2));
          Serial.println("11 ");
          break;
        case 11:
          SwitchLED(Zwoelf, (sizeof(Zwoelf) / 2));
          Serial.println("12 ");
          break;
        case 12:
          SwitchLED(Eins, (sizeof(Eins) / 2));
          Serial.println("1 ");
          break;
      }
    }
  }
  FastLED.show();
}

void CheckDST(void) {
  //Funktion zum Überprüfen ob Sommer oder Winterzeit
  Tag = day();
  Monat = month();

  WochenTag = (weekday() - 1);
  int Hilf = Tag - WochenTag;
  if (Monat <= 2 || Monat >= 11)
    DST = false; // Winterzeit

  if (Monat >= 4 && Monat <= 9)
    DST = true;    // Sommerzeit

  if ((Monat == 3) && (Hilf >= 25))
    DST = true; // Beginn der Sommerzeit

  if ((Monat == 10) && (Hilf >= 25))
    DST = false; // Beginn der Winterzeit

  if (DST == true) {
    if (Stunde <= 11)
      Stunde = Stunde + 1;
    else if (Stunde > 12)
      Stunde = Stunde - 12 + 1;
    else if (Stunde == 12)
      Stunde = 1;
    else if (Stunde == 24)
      Stunde = 1;
    //Stunde += 1;
  }
}

// Pattern 1 - White light, all LEDs in the strip are white
void pattern1() {
  leds[i].setRGB(255, 255, 255); // White
  FastLED.show();
}

// Pattern 2 - Red light, all LEDs in the strip are white
void pattern2() {
  leds[i].setRGB(255, 0, 0); // Red
  FastLED.show();
}

void loop() {
  /* Schleifen Funktion, die die Uhrzeit ausliest
      und dann in die entsprechende Funktionen übergibt.
  */

  stateH = digitalRead(bH);
  stateM = digitalRead(bM);
  //Minute = minute();
  if (stateH == LOW) {
    Std++;
    if (Std >24)
    {
      Std =0;
    }
  }
  if (stateM == LOW) {
    Minute++;
    if (Minute >59)
    {
      Minute =0;
    }
  }



  //int Std = hour();

  if (Std > 12)
    Stunde = Std - 12;
  else if (Std == 0)
    Stunde = 12;
  else
    Stunde = Std;



  CheckDST();
  displaytime();
  FastLED.delay(250);
  Serial.println(Stunde);
  Serial.println(Minute);
  /*
  //Button
  // read that state of the pushbutton value;
  int buttonVal = digitalRead(buttonPin);
  if (buttonVal == LOW && oldButtonVal == HIGH) {
    // button has just been pressed
    lightPattern = lightPattern + 1;
  }
  if (lightPattern > nPatterns) lightPattern = 1;
  oldButtonVal = buttonVal;

  switch (lightPattern) {
    case 1:
      pattern1();
      break;
    case 2:
      pattern2();
      break;
  }
  */
}

