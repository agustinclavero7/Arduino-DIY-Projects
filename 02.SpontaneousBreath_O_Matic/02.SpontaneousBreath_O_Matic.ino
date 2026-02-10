#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SMOTOR2_PIN 6
#define SERVO_RESTORE_CONS 15
#define SERVO_STEP 1
#define SERVO_STEP_DELAY 2

#define LCD_RS 13
#define LCD_E  12
#define LCD_D4 11
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8

#define LED_PIN 7

#define ADJUST_PIN  A0
#define OK_BUTTON   4
#define CALIBRATION_PIN 2
#define POTE_FILTER 10

#define NUMBER_OF_SAMPLES 5

#define EEPROM_CAL_ADRESS 100

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

//Pause
unsigned long pauseTimeStart = millis();
int pauseDelay = 150;
bool pauseFlag = false;

void setup(){
	Serial.begin(115200);
  lcd.begin(16,2);
  pinMode(OK_BUTTON,INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(CALIBRATION_PIN, INPUT);
  actualPoteValue = analogRead(ADJUST_PIN);
  prevPoteValue = actualPoteValue;
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
  initialPos = EEPROM.read(EEPROM_CAL_ADRESS);
  endPos = initialPos + SERVO_RESTORE_CONS;
}

void loop(){
  unsigned long timeNow = millis();
  //Main function → move the servos Or enter calibration screen
  if(isCalibration){
    calibrationRoutine();
    servoTimeStart = timeNow - servoDelay;
    prevPoteValue = actualPoteValue;
  }
  else if (pauseFlag){
    //Serial.println("Pause mode");
    pauseState();
    actualPoteValue = analogRead(ADJUST_PIN);
    int howMuchPoteMove = abs(prevPoteValue - actualPoteValue);
    if(howMuchPoteMove > POTE_FILTER){
      pauseFlag = false;
      lcd.clear();
    }
  }
  else{
    //Move servos and show data.
    if(timeNow - servoTimeStart > servoDelay){
      int servoTimePeriod = timeNow - servoTimeStart;
      servoMove();
      calculateBpm(servoTimePeriod);
      showBpmOnLcd();
      servoTimeStart += servoDelay;
    }
    //Change Bpm w/ potentiometer
    actualPoteValue = analogRead(ADJUST_PIN);
    int howMuchPoteMove = abs(prevPoteValue - actualPoteValue);
    if(howMuchPoteMove > POTE_FILTER){
      bpmChangeFlag = true;
    }
    else if (digitalRead(OK_BUTTON) == LOW && bpmChangeFlag == false){
      if(timeNow - pauseTimeStart > pauseDelay){
        pauseTimeStart+= pauseDelay;
        pauseFlag = true;
        lcd.clear();
      }
    }
    changeBreathRate(actualPoteValue);
  
    //Blink Led
    if(timeNow - ledTimerStart > ledDelay){
      ledTimerStart += ledDelay;
      ledBlink();
    }
  }
}
