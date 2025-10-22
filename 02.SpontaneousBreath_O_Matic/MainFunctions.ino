void servoMove(){
  if(oneWay){
    for(int i = initialPos; i < endPos; i+= step){
      //sMotor1.write(i);
      sMotor2.write(i);
      delayMicroseconds(3);    
    }
    oneWay = false;
  }
  else{
    for(int i = endPos; i > initialPos; i-= step){
      //sMotor1.write(i);
      sMotor2.write(i);
      delayMicroseconds(3);
    }
    oneWay = true;
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

void changeBreathRate(int poteValue){
  if(bpmChangeFlag){
    int configuration = 5 + poteValue / 20;
    lcd.setCursor(0,1);
    lcd.print(configuration);
    lcd.print("    ");
    if (digitalRead(OK_BUTTON) == HIGH){
      servoDelay = (60.0 / configuration)*1000;
      // Serial.println(servoDelay);
      // Serial.println(configuration);
      ledDelay = servoDelay / 10;
      prevPoteValue = poteValue;
      bpmChangeFlag = false;
      lcd.setCursor(0,1);
      lcd.print("                            ");
    }
  }
}