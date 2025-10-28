void servoMove(){
<<<<<<< HEAD
  for(int i = initialPos; i < endPos; i+= SERVO_STEP){
    //sMotor1.write(i);
    sMotor2.write(i);
    delayMicroseconds(4000);    
  }
  for(int i = endPos; i > initialPos; i-= SERVO_STEP){
    //sMotor1.write(i);
    sMotor2.write(i);
    delayMicroseconds(4000);
  }
=======
 // if(oneWay){
    for(int i = initialPos; i < endPos; i+= step){
      //sMotor1.write(i);
      sMotor2.write(i);
      delayMicroseconds(5000);
    }
    oneWay = false;
  //}
  //else{
    for(int i = endPos; i > initialPos; i-= step){
      //sMotor1.write(i);
      sMotor2.write(i);
      delayMicroseconds(5000);
    }
    oneWay = true;
  //}
>>>>>>> 4b70da726d8c9ba557fe5508a189eb46189c32a1
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
    int configuration = 5 + poteValue / 20;
    lcd.setCursor(0,1);
    lcd.print(configuration);
    lcd.print("    ");
    if (digitalRead(OK_BUTTON) == HIGH){
      servoDelay = (60.0 / configuration)*1000;
<<<<<<< HEAD
      ledDelay = servoDelay / 2;
=======
      // Serial.println(servoDelay);
      // Serial.println(configuration);
      ledDelay = servoDelay/2;
>>>>>>> 4b70da726d8c9ba557fe5508a189eb46189c32a1
      prevPoteValue = poteValue;
      bpmChangeFlag = false;
      lcd.setCursor(0,1);
      lcd.print("                            ");
    }
  }
}