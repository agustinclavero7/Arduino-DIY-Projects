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
#define LEVEL_3 98.5
#define LEVEL_2  65.5
#define LEVEL_1  32.8

//Gas sensor
unsigned long gasTimerStart = millis();
int gasTimerDelay = 50;
volatile bool isGasactivated = false;

//Nivel de agua
int ledArray [5] ={LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN,LED5_PIN};

//Botón de función
unsigned long debounceTimer = millis();
byte debounceDelay = 50;
byte function = 1; //1 medidor de gas 
                  // 2 medición de agua

void gasInterrupt(){
  isGasactivated = true;
}

double calculateWaterLevel(){
}

void setup() {
  Serial.begin(115200);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  for (i=0;i<5;i++)
    pinMode(ledArray[i],OUTPUT);
  pinMode(FUNCTION_PIN,INPUT_PULLUP);
  digitalWrite(BUZZER_PIN,LOW);
  attachInterrupt(digitalPinToInterrupt(FUNCTION_PIN),
                                        gasInterrupt,CHANGE);
}

void loop() {
  unsigned long timeNow = millis();
  //leer estado de botón
  if (debounceTimer - timeNow > debounceDelay){
    if(digitalRead(FUNCTION_PIN) == LOW){
      debounceTimer += debounceDelay;
    }
  }

  if(isGasactivated){
  //Función detector de gas
    if(timeNow - gasTimerStart > gasTimerDelay){
      gasTimerStart += gasTimerDelay;
      int gasLevel = analogRead(ANALOG_GAS_PIN);
      digitalGasState = digitalRead(DIGITAL_GAS_PIN);
      digitalWrite(LED_BUILTIN,digitalGasState);
      digitalWrite(BUZZER_PIN,digitalGasState);
    }
  }
  else {
  //Función medidor de tanque de agua
  }
}
