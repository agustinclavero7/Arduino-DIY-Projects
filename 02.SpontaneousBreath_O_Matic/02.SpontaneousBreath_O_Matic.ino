#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define SMOTOR2_PIN         6
#define SERVO_FW           20
#define SERVO_STEP          1
#define SERVO_STEP_DELAY    3

#define LCD_RS  13
#define LCD_E   12
#define LCD_D4  11
#define LCD_D5  10
#define LCD_D6  9
#define LCD_D7  8

#define LED_PIN   41

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
volatile int encoderDebounceDelay = 100;
unsigned long changeBpmTimer = millis();
byte changeBpmDelay = 100;

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
  pinMode(ENCODER_A_PIN,INPUT);
  pinMode(ENCODER_B_PIN,INPUT);
  pinMode(ENCODER_SW_PIN,INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
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
  initialPos = EEPROM.read(EEPROM_CAL_ADRESS);
  endPos = initialPos + SERVO_FW;
}

void loop(){
  unsigned long timeNow = millis();
  //Main function → move the servos Or enter calibration screen
  if(isCalibration)
    calibrationRoutine(actualEncoderPosition);
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
    timeNow = millis();
    if(timeNow - changeBpmTimer > changeBpmDelay){
      changeBpmTimer += changeBpmDelay;
      if(actualEncoderPosition != lastEncoderPosition)
        changeBreathRate(actualEncoderPosition);
    }
    // //Blink Led
    // if(timeNow - ledTimerStart > ledDelay){
    //   ledTimerStart += ledDelay;
    //   ledBlink();
    // }
  }
}