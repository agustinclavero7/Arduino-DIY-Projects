#define ANALOG_GAS_PIN A0
#define DIGITAL_GAS_PIN 53
#define BUZZER_PIN 51

unsigned long gasTimerStart = millis();
int gasTimerDelay = 50;

int digitalGasState;

void setup() {
  Serial.begin(115200);
  pinMode(DIGITAL_GAS_PIN,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  digitalWrite(BUZZER_PIN,LOW);
}

void loop() {
  unsigned long timeNow = millis();
  if(timeNow - gasTimerStart > gasTimerDelay){
    gasTimerStart += gasTimerDelay;
    int gasLevel = analogRead(ANALOG_GAS_PIN);
    Serial.println(gasLevel);
  }

  digitalGasState = digitalRead(DIGITAL_GAS_PIN);
  digitalWrite(LED_BUILTIN,!digitalGasState);
  digitalWrite(BUZZER_PIN,!digitalGasState);


}

