#define WATERTANK_WIDTH  67.0
#define WATERTANK_DEPTH  70.0
#define WATERTANK_HEIGHT 28.0
#define CM3_TO_L         0.001

#define ANALOG_GAS_PIN  A0
#define DIGITAL_GAS_PIN 3
#define BUZZER_PIN      4
#define FUNCTION_PIN    5
#define LED1_PIN        6    
#define LED2_PIN        7
#define LED3_PIN        8
#define LED4_PIN        9
#define LED5_PIN        10

#define LEVEL_5   
#define LEVEL_4
#define LEVEL_3  98.5
#define LEVEL_2  65.5
#define LEVEL_1  32.8

//Gas sensor
unsigned long lastIRStime = 0;
int IRSdelay = 25;
volatile bool isGasactivated = false;
bool lastGasState = false;
unsigned long gasTimer = 0;
int gasDelay = 5000;
 

//Nivel de agua
int ledArray [5] ={LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN,LED5_PIN};

void gasInterrupt(){
  unsigned long currentISRtime = millis ();
  if (currentISRtime - lastIRStime > IRSdelay)
  { 
    lastIRStime = currentIRStime;
    if(isGasactivated)
      isGasactivated = false;
    else
      isGasactivated = true;
  }
}

double calculateWaterLevel(){

}

void setup() {
  Serial.begin(115200);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  for (int i=0;i<5;i++)
    pinMode(ledArray[i],OUTPUT);
  pinMode(FUNCTION_PIN,INPUT_PULLUP);
  digitalWrite(BUZZER_PIN,LOW);
  attachInterrupt(digitalPinToInterrupt(FUNCTION_PIN),
                                        gasInterrupt,CHANGE);
}

void loop() {
  unsigned long timeNow = millis();
  
  //Función detector de gas
  if(isGasactivated){
    if(lastGasState != isGasactivated){
      lastGasState = isGasactivated;
      for 
    }

    if(timeNow - gasTimer > gasDelay){
      gasTimer = timeNow;
      digitalGasState = digitalRead(DIGITAL_GAS_PIN);
      digitalWrite(BUZZER_PIN,digitalGasState);
    }
  }
  else {
    lastGasState = false;
  //Función medidor de tanque de agua
  }
}
