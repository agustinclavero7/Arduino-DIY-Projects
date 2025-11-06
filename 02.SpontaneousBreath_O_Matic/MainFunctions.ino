void encoderInterrupt(){
  unsigned long timeNow = millis();
  if(timeNow - lastInterruptTime > encoderDebounceDelay){
    bpmChangeFlag = true;
    lastInterruptTime = timeNow;
    if(digitalRead(ENCODER_B_PIN) == HIGH)
      actualEncoderPosition++;
    else
      actualEncoderPosition--;

    if(isCalibration)
      actualEncoderPosition = min(180, max (0, actualEncoderPosition));
    else
      actualEncoderPosition = min(60, max (0, actualEncoderPosition));
  }
}

void changeBreathRate(int encoderValue){
  if(bpmChangeFlag){
    lcd.setCursor(0,1);
    lcd.print(encoderValue);
    lcd.print("    ");
<<<<<<< HEAD
    if (digitalRead(ENCODER_SW_PIN) == HIGH){
=======
    if (digitalRead(ENCODER_SW_PIN) == LOW){
>>>>>>> aa8dde34652e42451148aa4cfc60de8846e6372e
      servoDelay = (60.0 / encoderValue)*1000;
      ledDelay = servoDelay / 2;
      bpmChangeFlag = false;
      lcd.setCursor(0,1);
      lcd.print("                            ");
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
    delayMicroseconds(SERVO_STEP_DELAY);    
  }
  for(int i = endPos; i > initialPos; i-= SERVO_STEP){
    sMotor2.write(i);
    delayMicroseconds(SERVO_STEP_DELAY);
  }
}

void calculateBpm(int period){
  monitorBpmArray[index] =(60.0/(period))*1000; 
  if(index == NUMBER_OF_SAMPLES - 1)
    index = 0;
  else
    index++;
}