#include <Servo.h>
#include <LiquidCrystal.h>

#define SMOTOR1_PIN 3
#define SMOTOR2_PIN 4

#define LCD_RS 53
#define LCD_E  51
#define LCD_D4 49
#define LCD_D5 47
#define LCD_D6 45
#define LCD_D7 43

#define LED_PIN A15

#define ADJUST_PIN  A0
#define OK_BUTTON   A1
#define CALIBRATION_PIN 2
#define POTE_FILTER 5

#define NUMBER_OF_SAMPLES 5

//Servo sMotor1;
Servo sMotor2;

LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,
                 LCD_D5,LCD_D6,LCD_D7);

// Servo
unsigned long servoTimeStart = millis();
unsigned long servoDelay = 5000;
bool oneWay = true;
int step = 1;
// Add read value from EEPROM
int initialPos = 75;
int endPos = 120;

//Adjust settings
int prevPoteValue;
int actualPoteValue;
bool bpmChangeFlag = false;

//LCD
double monitorBpmArray[NUMBER_OF_SAMPLES] ={12.0,12.0,12.0,
                                            12.0,12.0};
int index = 0;

//LED
unsigned long ledTimerStart = millis();
unsigned long ledDelay = 200;
int ledState = LOW;

//Calibration
volatile bool isCalibration = false;

void setup(){
	Serial.begin(115200);
  	Serial.setTimeout(5);
  	lcd.begin(16,2);
  	pinMode(OK_BUTTON,INPUT);
  	pinMode(LED_PIN, OUTPUT);
	  actualPoteValue = analogRead(ADJUST_PIN);
  	prevPoteValue = actualPoteValue;
    //sMotor1.attach(SMOTOR1_PIN);
	  sMotor2.attach(SMOTOR2_PIN);
  	attachInterrupt(digitalPinToInterrupt(CALIBRATION_PIN),
                              calibrationInterrupt,RISING);
  	lcd.print("  Spontaneous   ");
    lcd.setCursor(0,1);
    lcd.print("    Breather    ");
  	delay(1000);
  	lcd.clear();
}

void loop(){
  unsigned long timeNow = millis();
  actualPoteValue = analogRead(ADJUST_PIN);
  //Main function → move the servos Or enter calibration screen
  if(isCalibration){
    calibrationRoutine(actualPoteValue);
    servoTimeStart = timeNow - servoDelay;
    prevPoteValue = actualPoteValue;
  }
  else{
    //Change Bpm w/ potentiometer
    int howMuchPoteMove = abs(prevPoteValue - actualPoteValue);
    if(howMuchPoteMove > POTE_FILTER){
      bpmChangeFlag = true;
    }
    changeBreathRate(actualPoteValue);
    
    //Move servos and show data.
    showBpmOnLcd();
    if(timeNow - servoTimeStart > servoDelay){
      monitorBpmArray[index] =(60.0/(timeNow-servoTimeStart))*1000; 
      if(index == NUMBER_OF_SAMPLES - 1)
       index = 0;
      else
       index++;
      servoMove();
      servoTimeStart += servoDelay;
      //Serial.println(servoDelay);
    }
  }
  
  //Blink Led
  if(timeNow - ledTimerStart > ledDelay){
    ledTimerStart += ledDelay;
   	if(ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(LED_PIN,ledState);
  }
}