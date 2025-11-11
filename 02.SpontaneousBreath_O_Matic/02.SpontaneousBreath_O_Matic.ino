#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SMOTOR2_PIN         6
#define SERVO_RESTORE_CONS 20
#define SERVO_STEP          1
#define SERVO_STEP_DELAY    3

#define LCD_RS  53
#define LCD_E   51
#define LCD_D4  49
#define LCD_D5  47
#define LCD_D6  45
#define LCD_D7  43

#define LED_PIN   41
#define PAUSE_PIN  7

#define ENCODER_A_PIN   3
#define ENCODER_B_PIN   4
#define ENCODER_SW_PIN  5
#define CALIBRATION_PIN 2

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
volatile int actualEncoderPosition = 12;
volatile int lastEncoderPosition = actualEncoderPosition;
volatile unsigned long lastInterruptTime = millis();
volatile int encoderDebounceDelay = 5;

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
volatile bool pauseStateFlag = false;

void setup(){
	Serial.begin(115200);
  lcd.begin(16,2);
  pinMode(ENCODER_A_PIN,INPUT);
  pinMode(ENCODER_B_PIN,INPUT);
  pinMode(ENCODER_SW_PIN,INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PAUSE_PIN,INPUT);
  sMotor2.attach(SMOTOR2_PIN, 1000, 2000);
  attachInterrupt(digitalPinToInterrupt(CALIBRATION_PIN),
                            calibrationInterrupt,RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN),
                                   encoderInterrupt,LOW);
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
  //Main function → move the servos Or enter calibration screen
  if(isCalibration){
    calibrationRoutine(actualEncoderPosition);
  }
  else if(pauseStateFlag){
    pauseFunction();
  }
  else{
    //Move servos and show data.
    if(timeNow - servoTimeStart > servoDelay){
      int servoTimePeriod = timeNow - servoTimeStart;
      calculateBpm(servoTimePeriod);
      showBpmOnLcd();
      servoMove();
      servoTimeStart += servoDelay;
    }
    //Change Bpm w/ encoder
    if(actualEncoderPosition != lastEncoderPosition){
      changeBreathRate(actualEncoderPosition);
    }
    if(digitalRead(PAUSE_PIN) == HIGH){
      pauseStateFlag = true;
    }
      //Blink Led
    if(timeNow - ledTimerStart > ledDelay){
      ledTimerStart += ledDelay;
      ledBlink();
    }
  }
}