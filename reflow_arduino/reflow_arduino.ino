#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31856.h>

//on_off用スイッチ
const int swPin = 3;
volatile bool swState = false;

//スイッチング用リレー
const int relayPin = 2;


//OLEDの設定
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
const int oledRestPin = 0;   // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, oledRestPin);

//温度検知設定
const int thermoCS = 1;
const int maxTemp = 270;
int intervalTime = 200;
Adafruit_MAX31856 max1 = Adafruit_MAX31856(thermoCS, 10, 9, 8);  // Use software SPI: CS, DI, DO, CLK

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  pinMode(swPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(swPin), chageOnOff, RISING);

  display.clearDisplay();
}

void loop() {
  /* Start MAX31856 and read temperatures */
  max1.begin();
  max1.setThermocoupleType(MAX31856_TCTYPE_K);

  float nowTemp = max1.readThermocoupleTemperature();
  Serial.println(nowTemp);
  Serial.println(swState);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print(nowTemp);
  display.println("degC");
  
  if(swState == false){
    digitalWrite(relayPin, LOW);
    display.println("OFF");
  }else if(nowTemp > maxTemp){
    digitalWrite(relayPin, LOW);
    display.println("OFF");
    swState = false;
  }else{
    digitalWrite(relayPin, HIGH);
    display.println("ON");
  }
  
  display.display();

  SPI.end();
  delay(intervalTime);
}


//reverse state
void chageOnOff() {
  swState = !swState;
}