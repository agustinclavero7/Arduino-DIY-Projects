void calibrationInterrupt(){
  if(isCalibration == false){
    isCalibration = true;
  }
  actualEncoderPosition = EEPROM.read(EEPROM_CAL_ADRESS);
}

void calibrationRoutine(int motorPosition){
  lcd.setCursor(0,0);
  lcd.print("Cal Screen             ");
  lcd.setCursor(0,1);
  lcd.print("Motor Pos: ");
  lcd.print(motorPosition);
  lcd.print("           ");
  sMotor2.write(motorPosition);
  Serial.print(" Motor ");
  Serial.println(motorPosition);
  if(digitalRead(ENCODER_SW_PIN) == LOW){
    lastTimeSw += pauseDelay;
    initialPos = motorPosition;
    EEPROM.write(EEPROM_CAL_ADRESS,initialPos);
    endPos = initialPos + SERVO_FW;
    isCalibration = false;
    actualEncoderPosition = lastEncoderPosition;
    servoTimeStart += servoDelay;
    lcd.clear();
  } 
}

