#include <Servo.h>
#include <LiquidCrystal.h>

#define SMOTOR1_PIN 3
#define SMOTOR2_PIN 4

#define LCD_RS A5
#define LCD_E  A4
#define LCD_D4 22
#define LCD_D5 24
#define LCD_D6 26
#define LCD_D7 28

#define LED_PIN A2

#define OK_BUTTON   A0
#define ADJUST_PIN  A1
#define CALIBRATION_PIN 2

#define NUMBER_OF_SAMPLES 3

Servo sMotor1;
Servo sMotor2;

LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,
                 LCD_D5,LCD_D6,LCD_D7);

// Servo
unsigned long servoTimeStart = millis();
unsigned long servoDelay = 5000;
bool oneWay = true;
int step = 1;
int initialPos = 75;
int endPos = 120;

//Adjust settings
unsigned long debounceTimeStart = millis();
unsigned long debounceDelay = 100;
int configBpm = (60 /servoDelay) * 1000;
int prevPoteValue = analogRead(ADJUST_PIN);
int actualPoteValue = prevPoteValue;
bool bpmchangeflag = false;

//LCD
double monitorBpmArray[NUMBER_OF_SAMPLES] ={12,12,12};
 
//LED
unsigned long ledTimerStart = millis();
unsigned long ledDelay = 200;
int ledState = LOW;

//Calibration
volatile bool isCalibration = false;
volatile bool firstOrSecondParameter = false;

void servoMove(){
  if(oneWay){
    for(int i = initialPos; i <= endPos; i+= step){
      sMotor1.write(i);
      sMotor2.write(i);
      delay(5);    
    }
    oneWay = false;
  }
  else{
    for(int i = endPos; i >= initialPos; i-= step){
      sMotor1.write(i);
      sMotor2.write(i);
      delay(5);
    }
    oneWay = true;
  }
}

void showBpmOnLcd(){
  double bpm = 0;
  for (int i = 0; i < NUMBER_OF_SAMPLES; i++){
    bpm += monitorBpmArray[i]; 
  }
  bpm = bpm / 3;
  lcd.setCursor(0,0);
  lcd.print(bpm);
  lcd.print(" bpm");
}

void changeBreathRate(int poteValue){
  if(bpmchangeflag){
    int configuration = 5 + poteValue / 4;
    lcd.setCursor(0,1);
    lcd.print(configuration);
    if (digitalRead(OK_BUTTON) == HIGH){
      configBpm = configuration;
      servoDelay = (60.0 / configBpm)*1000;
      ledDelay = servoDelay / 10;
      prevPoteValue = poteValue;
      bpmchangeflag = false;
      lcd.setCursor(0,1);
      lcd.print("                            ");
    }
  }
}

void calibrationInterrupt(){
  if(isCalibration == false){
    isCalibration = true;
  }
  else if (isCalibration == true && firstOrSecondParameter == false)
    firstOrSecondParameter = true;
  else if (isCalibration == true && firstOrSecondParameter == true){
    isCalibration = false;
    firstOrSecondParameter = false;
    lcd.clear();
  }
    
}

void calibrationRoutine(){
  lcd.setCursor(0,0);
  lcd.print("Cal Screen             ");
  if (firstOrSecondParameter == false){
    lcd.setCursor(0,1);
    lcd.print("Input start pos. ");
    if (Serial.available() > 0){
      int input = Serial.parseInt();
      if(input < 0 || input > 180)
        initialPos = 0;
      else
        initialPos = input;
      sMotor1.write(initialPos);
      sMotor2.write(initialPos);
    } 
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("Input end pos.     ");
    if (Serial.available() > 0){
      int input = Serial.parseInt();
      if(input < 0 || input > 180)
        endPos = 0;
      else
        endPos = input;
      sMotor1.write(endPos);
      sMotor2.write(endPos);
    }
  }
}

void setup(){
	Serial.begin(115200);
  	Serial.setTimeout(5);
  	lcd.begin(16,2);
  	pinMode(OK_BUTTON,INPUT);
  	pinMode(LED_PIN, OUTPUT);
    sMotor1.attach(SMOTOR1_PIN);
	  sMotor2.attach(SMOTOR2_PIN);
  	attachInterrupt(digitalPinToInterrupt(CALIBRATION_PIN),
                    calibrationInterrupt,RISING);
  	lcd.print("  SpontBreath       ");
    lcd.setCursor(0,1);
    lcd.print("    O-Matic         ");
  	delay(2000);
  	lcd.clear();
}

void loop(){
  unsigned long timeNow = millis();
  actualPoteValue = analogRead(ADJUST_PIN);
  if(isCalibration){
    calibrationRoutine();
  }
  else if(timeNow - servoTimeStart > servoDelay){
    for (int i=0; i < NUMBER_OF_SAMPLES;i++){
      monitorBpmArray[i] = (60.0 /servoDelay) * 1000;
    }
    showBpmOnLcd();
    servoMove();
    servoTimeStart += servoDelay;
    //Serial.println(servoDelay);
  }
  
  if(prevPoteValue != actualPoteValue){
    bpmchangeflag = true;
  }
  changeBreathRate(actualPoteValue);
  
  if(timeNow - ledTimerStart > ledDelay){
    ledTimerStart += ledDelay;
   	if(ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(LED_PIN,ledState);
  }
}