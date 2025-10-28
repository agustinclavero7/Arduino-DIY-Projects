#include <LiquidCrystal.h>

#define WATERTANK_WIDTH  67.0
#define WATERTANK_DEPTH  70.0
#define WATERTANK_HEIGHT 28.0
#define CM3_TO_L 0.001

#define ANALOG_GAS_PIN  A0
#define DIGITAL_GAS_PIN 53
#define BUZZER_PIN      51

#define ECCHO_PIN   2
#define TRIGGER_PIN 23

#define LCD_RS 49
#define LCD_E  47
#define LCD_D4 45
#define LCD_D5 43
#define LCD_D6 41
#define LCD_D7 39

#define LED_HIGH 13
#define LED_MED  12 
#define LED_LOW  11

#define LEVEL_HIGH 98.5
#define LEVEL_MED  65.5
#define LEVEL_LOW  32.8

//Lcd Screen
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4,
                   LCD_D5, LCD_D6, LCD_D7);

//Gas sensor
unsigned long gasTimerStart = millis();
int gasTimerDelay = 50;
int digitalGasState;

//Water level measuring
unsigned long triggerTimerStart = millis();
int triggerTimerDelay = 60;
volatile unsigned long ecchoStart;
volatile unsigned long ecchoFinish;
volatile bool waterLevelMeasureReady = false;

//Led indicator
int LedArray[3] = {LED_LOW,LED_MED,LED_HIGH};
int LedstateLow[3] = {HIGH,LOW,LOW};
int LedstateMed[3] = {HIGH,HIGH,LOW};
int LedstateHigh[3] = {HIGH,HIGH,HIGH};
int LedstateOvf[3] = {LOW,LOW,LOW};
int ovfTimerDelay = 1000;
unsigned long ovfTimerStart = millis();

void activateTrigger(){
  digitalWrite(TRIGGER_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN,LOW);
}

void ultrasonicSensorInterruot(){
  if(digitalRead(ECCHO_PIN) == HIGH)
    ecchoStart = micros();
  else{
    ecchoFinish = micros();
    waterLevelMeasureReady = true;
  }

}

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(LED_HIGH,OUTPUT);
  pinMode(LED_MED,OUTPUT);
  pinMode(LED_LOW,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECCHO_PIN,INPUT);
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(BUZZER_PIN,LOW);
  attachInterrupt(digitalPinToInterrupt(ECCHO_PIN),
                                        ultrasonicSensorInterruot,CHANGE);

}

void loop() {
  unsigned long timeNow = millis();
  if(timeNow - gasTimerStart > gasTimerDelay){
    gasTimerStart += gasTimerDelay;
    int gasLevel = analogRead(ANALOG_GAS_PIN);
    //Serial.println(gasLevel);
  }
  digitalWrite(LED_BUILTIN,digitalGasState);
  digitalWrite(BUZZER_PIN,digitalGasState);
  digitalGasState = digitalRead(DIGITAL_GAS_PIN);

  if (timeNow - triggerTimerStart > triggerTimerDelay){
    triggerTimerStart += triggerTimerDelay;
    activateTrigger();
    if(waterLevelMeasureReady){
      waterLevelMeasureReady = false;
      double waterLevel = calculateWaterLevel();
      showWaterLevelOnLCD(waterLevel);
      if(waterLevel <= LEVEL_HIGH){
        lightWaterLevelLeds(waterLevel);
        digitalWrite(BUZZER_PIN,LOW);
      }
      else
        overFlowWarning(timeNow);
    }
  }
}
