void servoMove(){
  for(int i = initialPos; i < endPos; i+= SERVO_STEP){
    sMotor2.write(i);
    delay(SERVO_STEP_DELAY);    
  }
  for(int i = endPos; i > initialPos; i-= SERVO_STEP){
    sMotor2.write(i);
    delay(SERVO_STEP_DELAY);
  }
}

void calculateBpm(int period){
  monitorBpmArray[index] =(60.0/(period))*1000; 
  if(index == NUMBER_OF_SAMPLES - 1)
    index = 0;
  else
    index++;
}

void showBpmOnLcd(){
  double sumOfSamples = 0;
  for (int i = 0; i < NUMBER_OF_SAMPLES; i++){
    sumOfSamples += monitorBpmArray[i]; 
  }
  double bpm = sumOfSamples / NUMBER_OF_SAMPLES;
  lcd.setCursor(0,0);
  lcd.print(bpm);
  lcd.print(" bpm.             ");
}

void changeBreathRate(int poteValue){
  if(bpmChangeFlag){
    int configuration = map(poteValue,0,1023,5,60);
    lcd.setCursor(0,1);
    lcd.print(configuration);
    lcd.print("    ");
    if (digitalRead(OK_BUTTON) == LOW){
      pauseTimeStart = millis();
      servoDelay = (60.0 / configuration)*1000;
      ledDelay = servoDelay / 2;
      prevPoteValue = poteValue;
      bpmChangeFlag = false;
      lcd.setCursor(0,1);
      lcd.print("                            ");
    }
  }
}

void pauseState(){
  lcd.setCursor(0,0);
  lcd.print("Pausa");
  digitalWrite(LED_PIN,HIGH);
}

void ledBlink(){
  if(ledState == LOW)
    ledState = HIGH;
  else
    ledState = LOW;
  digitalWrite(LED_PIN,ledState);
}
