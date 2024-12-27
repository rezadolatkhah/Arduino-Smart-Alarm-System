#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <Arduino.h>
#include <FastLED.h>
#include <RTClib.h>
#include <TM1637Display.h>

#define CLK 8
#define DIO 9
#define VIB 10
#define ONEWIREBUS 11
#define Buzzer 12
#define LED_PIN 13
#define COLOR_ORDER GRB
#define BRIGHTNESS 125
#define MQs_Val 350

#define NUM_LEDS 8
CRGB leds[NUM_LEDS];
LiquidCrystal lcd (2,3,4,5,6,7);
OneWire oneWire(ONEWIREBUS);
DallasTemperature sensors(&oneWire);
RTC_DS3231 rtc;
TM1637Display display = TM1637Display(CLK, DIO);

uint8_t hue = 0;
const uint8_t hueIncrement = 1;
unsigned long lastChange = 0;
const unsigned long interval = 250;
const int MQ2 = A0;
int MQ2V = 0;
const int MQ7 = A1;
int MQ7V = 0;
float currentTemp = 0.0;
int VIB_Val = 0;

void setup(){

  Serial.begin(9600);
  sensors.setResolution(11);

  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  lcd.begin(16,2);  
  lcd.clear();
  digitalWrite(Buzzer, LOW);
  
  if (! rtc.begin()) {
        while (1);
  }
  if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
    display.setBrightness(5);
    display.clear();
}


void loop() {

  MQ2V = analogRead(MQ2);
  MQ7V = analogRead(MQ7);
  VIB_Val = digitalRead(VIB);
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastChange >= interval) {
  lastChange = currentMillis;
  hue += hueIncrement;
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
  FastLED.show();
  }
  
  if(VIB_Val == HIGH){
  Serial.print("Danger Detected on VIB");
  lcd.clear();
  digitalWrite(Buzzer, HIGH);
  lcd.setCursor(0,0);
  lcd.print("EARTHQUAKE! RUN!");
  lcd.setCursor(0,1);
  lcd.print("!DANGER IS HERE!");
  delay(5000);
  digitalWrite(Buzzer, LOW);
  lcd.clear();
  }
  DateTime now = rtc.now();
  int displaytime = (now.hour() * 100) + now.minute();
  display.showNumberDecEx(displaytime, 0b11100000, true);
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);

  lcd.setCursor(0,0);
  lcd.print(currentTemp);

  lcd.setCursor(8,0);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  lcd.print("    ");

  lcd.setCursor(0,1);
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());

  if (MQ2V >= MQs_Val){
  Serial.print("Danger Detected on MQ2  ");
  Serial.println(MQ2V);
  digitalWrite(Buzzer, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!!!FUME & GAS!!!");
  lcd.setCursor(0,1);
  lcd.print("!DANGER IS HERE!");
  delay(5000);
  lcd.clear();
  digitalWrite(Buzzer, LOW);
  }
  else if(MQ7V >= MQs_Val){
  Serial.print("Danger Detected on MQ7  ");
  Serial.println(MQ7V);
  digitalWrite(Buzzer, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!MONOXIDE & GAS!");
  lcd.setCursor(0,1);
  lcd.print("!DANGER IS HERE!");
  delay(5000);
  lcd.clear();
  digitalWrite(Buzzer, LOW);
  }
}
