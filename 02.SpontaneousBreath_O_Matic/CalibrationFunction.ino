void calibrationInterrupt(){
  if(isCalibration == false)
    isCalibration = true;
}

void calibrationRoutine(){
  int adjustPote = analogRead(ADJUST_PIN);
  int motorPosition = map(adjustPote, 0, 1023, 0, 180-SERVO_RESTORE_CONS);
  lcd.setCursor(0,0);
  lcd.print("Cal Screen             ");
  lcd.setCursor(0,1);
  lcd.print("Motor Pos: ");
  lcd.print(motorPosition);
  lcd.print("           ");
  //sMotor1.write(motorPosition);
  sMotor2.write(motorPosition);
  if(digitalRead(OK_BUTTON) == LOW)
  {
    initialPos = motorPosition;
    EEPROM.write(EEPROM_CAL_ADRESS,initialPos);
    endPos = initialPos + SERVO_RESTORE_CONS;
    isCalibration = false;
    lcd.clear();
  } 
}