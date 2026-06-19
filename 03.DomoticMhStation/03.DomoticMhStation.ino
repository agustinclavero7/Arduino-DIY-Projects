#include <Wire.h>             //I2C pins: nano → SDA A4, SCL A5
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WATERTANK_WIDTH  67.0
#define WATERTANK_DEPTH  70.0
#define WATERTANK_HEIGHT 28.0
#define CM3_TO_L 0.001

#define ANALOG_GAS_PIN  A0
#define DIGITAL_GAS_PIN 3
#define BUZZER_PIN      4
#define FUNCTION_PIN    5

#define ECCHO_PIN   2
#define TRIGGER_PIN 6

#define LEVEL_HIGH 98.5
#define LEVEL_MED  65.5
#define LEVEL_LOW  32.8

//Oled Screen
Adafruit_SSD1306 oledScreen(128,64,&Wire,4);

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

//Botón de función
unsigned long debounceTimer = millis();
byte debounceDelay = 50;
byte function = 1; //1 medidor de gas 
                  // 2 medición de agua

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

double calculateWaterLevel(){
  double waterDistance = (ecchoFinish - ecchoStart) / 58.0;
  double prevDistance;
  if (waterDistance > WATERTANK_HEIGHT)
    waterDistance = prevDistance;
  else
    prevDistance = waterDistance;
  double waterHeight = WATERTANK_HEIGHT - waterDistance;
  double waterVolume = waterHeight * WATERTANK_WIDTH * WATERTANK_DEPTH * CM3_TO_L;
  return waterVolume;
}

void showWaterLevelOled(double waterLevel){
  oledScreen.clearDisplay();
  oledScreen.setTextColor(WHITE);
  oledScreen.setTextSize(1);
  oledScreen.setCursor(0,0); 
  oledScreen.print("Nivel de agua");
  oledScreen.setTextSize(3);
  oledScreen.setCursor(20,30); 
  oledScreen.print(waterLevel);
  oledScreen.display();
}

// void overFlowWarning(unsigned long timeNow){
//   lcd.setCursor(0,1);
//   lcd.print("  !!Rebalse!!  ");
//   if (timeNow - ovfTimerStart > ovfTimerDelay){
//     ovfTimerStart += ovfTimerDelay;
//     if(digitalRead(LED_LOW) == LOW){
//       for(int i = 0; i < 3; i++)
//         digitalWrite(LedArray[i],LedstateHigh[i]);
//       digitalWrite(BUZZER_PIN,HIGH);
//     }
//     else{
//       for(int i = 0; i < 3; i++)
//         digitalWrite(LedArray[i],LedstateOvf[i]);
//       digitalWrite(BUZZER_PIN,LOW);
//     }
//   }
// }

void setup() {
  Wire.begin();
  oledScreen.begin(SSD1306_SWITCHCAPVCC,0x3C);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(FUNCTION_PIN,INPUT_PULLUP);
  pinMode(ECCHO_PIN,INPUT);
  digitalWrite(BUZZER_PIN,LOW);
  attachInterrupt(digitalPinToInterrupt(ECCHO_PIN),
                                        ultrasonicSensorInterruot,CHANGE);

  //Presentación
  oledScreen.clearDisplay();
  oledScreen.setTextColor(WHITE);
  oledScreen.setTextSize(1);
  oledScreen.setCursor(30,12); 
  oledScreen.print("Urraquita");
  oledScreen.setTextSize(2);
  oledScreen.setCursor(20,25);  
  oledScreen.print("Volando");
  oledScreen.display();
  delay(3000);

}

void loop() {
  unsigned long timeNow = millis();
  //leer estado de botón
  if (debounceTimer - timeNow > debounceDelay){
    if(digitalRead(FUNCTION_PIN) == LOW){
      debounceTimer += debounceDelay;
      if(function == 1){
        function++;
        oledScreen.clearDisplay();
        oledScreen.setTextSize(2);
        oledScreen.setCursor(30,12); 
        oledScreen.print("Agua");
        oledScreen.display();
        delay(1000);
      }
      else{
        function = 1;
        oledScreen.clearDisplay();
        oledScreen.setTextSize(2);
        oledScreen.setCursor(30,12); 
        oledScreen.print("Gas");
        oledScreen.display();
        delay(1000);
      }
    }
  }

  if(function == 1){
  //Función detector de gas
    if(timeNow - gasTimerStart > gasTimerDelay){
      gasTimerStart += gasTimerDelay;
      int gasLevel = analogRead(ANALOG_GAS_PIN);
      digitalGasState = digitalRead(DIGITAL_GAS_PIN);
      digitalWrite(LED_BUILTIN,digitalGasState);
      digitalWrite(BUZZER_PIN,digitalGasState);
    }
  }
  else if (function == 2){
  //Función medidor de tanque de agua
    if (timeNow - triggerTimerStart > triggerTimerDelay){
      triggerTimerStart += triggerTimerDelay;
      activateTrigger();
      if(waterLevelMeasureReady){
        waterLevelMeasureReady = false;
        int waterLevel = calculateWaterLevel();
        showWaterLevelOled(waterLevel);
        if(waterLevel <= LEVEL_HIGH){
          digitalWrite(BUZZER_PIN,LOW);
        }
        //else
          //overFlowWarning(timeNow);
      }
    }
  }
}
