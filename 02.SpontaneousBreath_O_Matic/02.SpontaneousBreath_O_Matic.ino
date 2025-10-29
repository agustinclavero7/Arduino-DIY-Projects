#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SMOTOR1_PIN 3
#define SMOTOR2_PIN 4
#define SERVO_RESTORE_CONS 30
#define SERVO_STEP 1
#define SERVO_STEP_DELAY 4000

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

#define EEPROM_CAL_ADRESS 100

//Servo sMotor1;
Servo sMotor2;

LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,
                 LCD_D5,LCD_D6,LCD_D7);

// Servo
unsigned long servoTimeStart = millis();
unsigned long servoDelay = 5000;
int initialPos;
int endPos;

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
unsigned long ledDelay = 2500;
int ledState = LOW;

//Calibration
volatile bool isCalibration = false;

void setup(){
	Serial.begin(115200);
  lcd.begin(16,2);
  pinMode(OK_BUTTON,INPUT);
  pinMode(LED_PIN, OUTPUT);
  actualPoteValue = analogRead(ADJUST_PIN);
  prevPoteValue = actualPoteValue;
  //sMotor1.attach(SMOTOR1_PIN);
  sMotor2.attach(SMOTOR2_PIN, 1000, 2000);
  attachInterrupt(digitalPinToInterrupt(CALIBRATION_PIN),
                            calibrationInterrupt,RISING);
  //Welcome message
  lcd.print("  Spontaneous   ");
  lcd.setCursor(0,1);
  lcd.print("    Breather    ");
  delay(1000);
  lcd.clear();
  //Restore Settings
  endPos = EEPROM.read(EEPROM_CAL_ADRESS);
  initialPos = endPos - SERVO_RESTORE_CONS;
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
    //Move servos and show data.
    showBpmOnLcd();
    if(timeNow - servoTimeStart > servoDelay){
      int servoTimePeriod = timeNow - servoTimeStart;
      calculateBpm(servoTimePeriod);
      servoMove();
      servoTimeStart += servoDelay;
    }
    //Change Bpm w/ potentiometer
    int howMuchPoteMove = abs(prevPoteValue - actualPoteValue);
    if(howMuchPoteMove > POTE_FILTER){
      bpmChangeFlag = true;
    }
    changeBreathRate(actualPoteValue);
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