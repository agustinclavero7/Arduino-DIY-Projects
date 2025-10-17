// FinalProject - Interactive obstacle detection system.
//01. Print measured distance on lcd. OK
//02. Blink an LCD according to proximity. OK
//03. Print a warning for objects closer to 50 cm. OK
//04. Block the app for objects closer than 10 cm (press the push button to unlock) and toogle aditional LED.
//    LEDS still Blink on the block state. Also luminosity works OK
//05. Get luminosity with photoresistor and light a led according to luminosity measured. OK
//06. Also, unlock application (see 04) with OK button on remote IR controller. OK
//07. Changue units of measure with remote controller. OK
//08. Save units settings on the EEPROM.
//09. With an IR controller command, print luminosity on screen. OK
//10. With an IR controller command, go to a reset screen and restore to default settings.
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

#define LED1_PIN 51
#define LED2_PIN 50
#define LED3_PIN 2
#define DISTANCE_TRESHOLD 10.0
#define BUTTON_PIN 52
#define USECCHO_PIN 3
#define USTRIGGER_PIN 4
#define PHOTORESISTOR_PIN A2
#define REMOTE_PIN 5

#define LCD_RS A0
#define LCD_E  A1
#define LCD_D4 24
#define LCD_D5 26
#define LCD_D6 28
#define LCD_D7 30

#define REMOTE_OK   28
#define REMOTE_0    25
#define REMOTE_1    69
#define REMOTE_2    70
#define REMOTE_UP   24
#define REMOTE_DOWN 82
#define REMOTE_9    9

#define UNITS_CM 0
#define UNITS_IN 1
#define CM_TO_IN 0.3937
#define EEPROM_ADRESS 51

//LCD display
LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,
                  LCD_D5,LCD_D6,LCD_D7);

//Ultrasonic sensor
unsigned long triggerStart = millis();
unsigned long triggerDelay = 100;
volatile unsigned long ecchoStart;
volatile unsigned long ecchoEnd;
volatile bool UsMeasureReady = false;
double previousDistance = 400;

//LEDS
unsigned long ledTimerStart = millis();
unsigned long ledTimerDelay = 500;
bool Led1State = false;

//Push button
unsigned long debounceTimerStart = millis();
int debounceDelay = 50;
int buttonState = LOW;

//Photorresistor
unsigned long luminositySampleStart = millis();
int luminositySampleDelay = 100;

int programState = 0; // 0 default, print distance
                      // 1 Program blocked
                      // 2 Show Luminosity
                      // 3 Restore settings

bool programBlocked = false;
bool reset = false;
int units = UNITS_CM;

void triggerSonicSensor(){
  digitalWrite(USTRIGGER_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(USTRIGGER_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(USTRIGGER_PIN,LOW);
}

void getUltrasonicDelayInterrupt(){
  if(digitalRead(USECCHO_PIN) == HIGH){
    ecchoStart = micros();
  }
  else{
    ecchoEnd = micros();
    UsMeasureReady = true;
  }
}

double calculateUltrasonicDistance(){
  double ecchoDelay = ecchoEnd - ecchoStart;
  double distance = ecchoDelay / 58.0;
  if(distance > 400)
    return previousDistance;
  else
    distance = 0.5 * distance + previousDistance * 0.5;
  previousDistance = distance;
  return distance;
}

void setLedBlinkDelay(double distance){
 if (programState != 1){
  ledTimerDelay = distance * 4;
 }
 else
  ledTimerDelay = 500;
}

void blinkLeds(){
  if (Led1State)
    Led1State = false;
  else
    Led1State = true;
  digitalWrite(LED1_PIN,Led1State);  
  if(programState == 1)
    digitalWrite(LED2_PIN,Led1State);
  else
    digitalWrite(LED2_PIN,LOW);
}

void getLuminosity(){
  int lightData = analogRead(PHOTORESISTOR_PIN);
  int luminosity = 255 - lightData / 4;
  analogWrite(LED3_PIN,luminosity);
}

void printUsDistance(double distance){
  lcd.setCursor(0,1);
  if(distance < 50)
    lcd.print("     WARNING    ");
  else
    lcd.print("No Obstacle.    ");

  lcd.setCursor(0,0);
  lcd.print("Dist.: ");
  if (units == UNITS_CM){
    lcd.print(distance);
    lcd.print(" cm        ");
  }
  else
    lcd.print(distance * CM_TO_IN);
    lcd.print(" in         ");
}

void blockProgram(){
  programBlocked = true;
  lcd.setCursor(0,0);
  lcd.print("Program blocked.");
  lcd.setCursor(0,1);
  lcd.print("Unlock w/ button");
}

void printLuminosity(){
  lcd.setCursor(0,0);
  lcd.print("Luminosity: ");
  lcd.print(analogRead(PHOTORESISTOR_PIN));
  lcd.print("      ");
  lcd.setCursor(0,1);
  lcd.print("                     ");
}

void handleIrRemoteCommands(){
  if(IrReceiver.decode()){
    IrReceiver.resume();
    int command = IrReceiver.decodedIRData.command;
    Serial.println(command);
    switch(command){
      case REMOTE_OK:{
        programState = 0;
        programBlocked = false;
      }
      case REMOTE_0:{
        toogleUnits();
        break;
      }
      case REMOTE_1:{
        programState = 2;
        break;
      }
      case REMOTE_UP:{
        programState = 3;
        break;
      }
       case REMOTE_DOWN:{
        programState = 0;
        break;
      }
      case REMOTE_9:
        reset = true;
        break;
      default:{
      //do nothing
      }
    }
  }
}

void toogleUnits(){
  if (units == UNITS_CM)
    units = UNITS_IN;
  else
    units = UNITS_CM;
  EEPROM.write(EEPROM_ADRESS,units);
  //Serial.println(units);
}

void factorySettings(){
  if(reset){
    lcd.setCursor(0,0);
    lcd.print("Settings have   ");
    lcd.setCursor(0,1);
    lcd.print("been reset.     ");
    reset = false;
    units = UNITS_CM;
    EEPROM.write(EEPROM_ADRESS,units);  
    delay(2000);
    programState = 0;
  }else{
    lcd.setCursor(0,0);
    lcd.print("Pess on 9 to    ");
    lcd.setCursor(0,1);
    lcd.print("reset settings. ");
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  IrReceiver.begin(REMOTE_PIN);
  pinMode(LED1_PIN,OUTPUT);
  pinMode(LED2_PIN,OUTPUT);
  pinMode(LED3_PIN,OUTPUT);
  pinMode(USTRIGGER_PIN,OUTPUT);
  pinMode(USECCHO_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(USECCHO_PIN),
                  getUltrasonicDelayInterrupt,CHANGE); 
  digitalWrite(LED1_PIN,LOW);
  digitalWrite(LED2_PIN,LOW);
  analogWrite(LED3_PIN,0);
  units = EEPROM.read(EEPROM_ADRESS);
  if (units == 255)
    units = UNITS_CM;
  //Welcome Message
  lcd.setCursor(0,0);
  lcd.print("  Interactive   ");
  lcd.setCursor(0,1);
  lcd.print(" Obstacle Gyzmo ");
  delay(2000);
}

void loop() {
  
  //Power on US trigger.
  unsigned long timeNow = millis();
  if(timeNow - triggerStart > triggerDelay){
    triggerSonicSensor();
    triggerStart += triggerDelay;
  }

  //Gets distance and check if program is blocked.
  if(UsMeasureReady){
    UsMeasureReady = false;
    double distance = calculateUltrasonicDistance();
    setLedBlinkDelay(distance);
    //Serial.println(distance);
    if(distance < DISTANCE_TRESHOLD)
      programState = 1;
  }

  //Blink Leds
  if(timeNow - ledTimerStart > ledTimerDelay){
    ledTimerStart += ledTimerDelay;
    blinkLeds();
  }

  //Check for push button to unlock program;
  if (timeNow - debounceTimerStart > debounceDelay){
    int newButtonState = digitalRead(BUTTON_PIN);
    if(buttonState != newButtonState){
      debounceTimerStart = timeNow;
      buttonState = newButtonState;
      if(buttonState == LOW){
        programState = 0;
        programBlocked = false;
        lcd.clear();
      }
    }
  }

  //LCD display state machine.
  if(programState == 0)
    printUsDistance(calculateUltrasonicDistance());
  else if (programState == 1)
    blockProgram();
  else if(programBlocked != true){ 
    if (programState == 2)
      printLuminosity();
    else if (programState == 3)
      factorySettings();
  }
  
  //Luminosity control
  if(timeNow - luminositySampleStart > luminositySampleDelay){
  luminositySampleStart += luminositySampleDelay;
  getLuminosity();
  }
  //IR remote control
  handleIrRemoteCommands();
}
