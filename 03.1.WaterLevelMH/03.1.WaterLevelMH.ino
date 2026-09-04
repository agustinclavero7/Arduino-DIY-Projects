#define WATERTANK_WIDTH  67.0
#define WATERTANK_DEPTH  70.0
#define WATERTANK_HEIGHT 28.0
#define KPA_TO_CM        10.193
#define Z0               0    //height offset
#define NUMBER_OF_LEDS   5

#define PRESSURE_SENSOR_PIN A0
#define DIGITAL_GAS_PIN     3
#define BUZZER_PIN          4
#define FUNCTION_PIN        5
#define LED1_PIN            6     
#define LED2_PIN            7
#define LED3_PIN            8
#define LED4_PIN            9
#define LED5_PIN            10

#define LEVEL_5  130 
#define LEVEL_4  110
#define LEVEL_3  98.5
#define LEVEL_2  65.5
#define LEVEL_1  32.8

//Gas sensor
unsigned long lastIRStime = 0;
int IRSdelay = 25;
volatile bool isGasactivated = false;
bool lastGasState = false;

//Nivel de agua
int ledArray [NUMBER_OF_LEDS] = {LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN,LED5_PIN};
int levels [NUMBER_OF_LEDS] = {LEVEL_1,LEVEL_2,LEVEL_3,LEVEL_4,LEVEL_5};
int ledsOn [NUMBER_OF_LEDS][NUMBER_OF_LEDS] = {{1,0,0,0,0},{1,1,0,0,0},{1,1,1,0,0},{1,1,1,1,0},{1,1,1,1,1}};
double kPa = 0;
double waterHeight = 0;

//Timers
unsigned long gasTimer = 0;
int gasDelay = 30000;
unsigned long waterTimer = 0;
int waterDelay = 10000;


void gasInterrupt(){
  unsigned long currentIRStime = millis ();
  if (currentIRStime - lastIRStime > IRSdelay){ 
    lastIRStime = currentIRStime;
    if(isGasactivated)
      isGasactivated = false;
    else
      isGasactivated = true;
  }
}

void getPressure(){
  int adc = analogRead(PRESSURE_SENSOR_PIN);
  kPa = (adc/1023 -0.04) / 0.09;
}

void calculateWaterLevel(){
  waterHeight = kPa * KPA_TO_CM;
  waterHeight += Z0;
  Serial.println(waterHeight);
}

void showLedsLevel(){
  int fila = 0;
  for(int i= 1; i<NUMBER_OF_LEDS; i++)
    if(waterHeight >= levels[i])
      fila++;

  for(int j=0; j< NUMBER_OF_LEDS; j++)
    digitalWrite(ledArray[j],ledsOn[fila][j]);
}

void setup() {
  Serial.begin(115200);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  analogReference(EXTERNAL);
  for (int i=0;i<NUMBER_OF_LEDS;i++)
    pinMode(ledArray[i],OUTPUT);
  pinMode(FUNCTION_PIN,INPUT_PULLUP);
  digitalWrite(BUZZER_PIN,LOW);
  attachInterrupt(digitalPinToInterrupt(FUNCTION_PIN),
                                        gasInterrupt,CHANGE);
}

void loop() {
  unsigned long timeNow = millis();
  
  if(isGasactivated){
    //Función detector de gas
    if(lastGasState != isGasactivated){
      lastGasState = isGasactivated;
    }

    if(timeNow - gasTimer > gasDelay){
      gasTimer = timeNow;
      digitalWrite(BUZZER_PIN,digitalRead(DIGITAL_GAS_PIN));
    }
  }
  else {
    lastGasState = false;
    //Función medidor de tanque de agua
    if(timeNow - waterTimer > waterDelay){
      waterTimer = timeNow;
      getPressure();
      calculateWaterLevel();
      showLedsLevel();

    }
  }
}
