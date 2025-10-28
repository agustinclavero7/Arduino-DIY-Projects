void calibrationInterrupt(){
  if(isCalibration == false)
    isCalibration = true;
}

void calibrationRoutine(int adjustPote){
  int motorPosition = adjustPote / 5.7;
  lcd.setCursor(0,0);
  lcd.print("Cal Screen             ");
  lcd.setCursor(0,1);
  lcd.print("Motor Pos: ");
  lcd.print(motorPosition);
  lcd.print("           ");
  //sMotor1.write(motorPosition);
  sMotor2.write(motorPosition);
  if(digitalRead(OK_BUTTON) == HIGH)
  {
    endPos = motorPosition;
    EEPROM.write(EEPROM_CAL_ADRESS,endPos);
<<<<<<< HEAD
    initialPos = endPos - SERVO_RESTORE_CON;
=======
    initialPos = endPos - 30;
>>>>>>> 4b70da726d8c9ba557fe5508a189eb46189c32a1
    isCalibration = false;
    lcd.clear();
  } 
}