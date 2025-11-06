void calibrationInterrupt(){
  if(isCalibration == false){
    isCalibration = true;
  }
}

void calibrationRoutine(int motorPosition){
  lcd.setCursor(0,0);
  lcd.print("Cal Screen             ");
  lcd.setCursor(0,1);
  lcd.print("Motor Pos: ");
  lcd.print(motorPosition);
  lcd.print("           ");
  sMotor2.write(motorPosition);
<<<<<<< HEAD
  if(digitalRead(ENCODER_SW_PIN) == HIGH)
=======
  if(digitalRead(ENCODER_SW_PIN) == LOW)
>>>>>>> aa8dde34652e42451148aa4cfc60de8846e6372e
  {
    endPos = motorPosition;
    EEPROM.write(EEPROM_CAL_ADRESS,endPos);
    initialPos = endPos - SERVO_RESTORE_CONS;
    isCalibration = false;
    actualEncoderPosition = lastEncoderPosition;
    lcd.clear();
  } 
}

