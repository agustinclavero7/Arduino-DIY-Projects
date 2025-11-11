void encoderInterrupt(){
  unsigned long timeNow = millis();
  pauseStateFlag = false;
  if(timeNow - lastInterruptTime > encoderDebounceDelay){
    lastInterruptTime = timeNow;
    if(digitalRead(ENCODER_B_PIN) == HIGH)
      actualEncoderPosition++;
    else
      actualEncoderPosition--;

    if(isCalibration)
      actualEncoderPosition = min(180, max (SERVO_RESTORE_CONS, actualEncoderPosition));
    else
      actualEncoderPosition = min(60, max (0, actualEncoderPosition));
  }
}

void changeBreathRate(int encoderValue){
  lcd.setCursor(0,1);
  lcd.print(encoderValue);
  lcd.print("    ");
  if (digitalRead(ENCODER_SW_PIN) == LOW){
    servoDelay = (60.0 / encoderValue)*1000;
    ledDelay = servoDelay / 2;
    lcd.setCursor(0,1);
    lcd.print(encoderValue);
    lcd.print("    ");
    if (digitalRead(ENCODER_SW_PIN) == LOW){
      servoDelay = (60.0 / encoderValue)*1000;
      ledDelay = servoDelay / 2;
      lcd.setCursor(0,1);
      lcd.print("                            ");
      lastEncoderPosition = actualEncoderPosition;
      lastTimeSw = millis();
    }
  }
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

void ledBlink(){
  if(ledState == LOW)
    ledState = HIGH;
  else
    ledState = LOW;
  digitalWrite(LED_PIN,ledState);
}

void pauseFunction(){
  digitalWrite(LED_PIN,HIGH);
  lcd.setCursor(0,1);
  lcd.print("Pause              ");
}