double calculateWaterLevel(){
  double waterDistance = (ecchoFinish - ecchoStart) / 58.0;
  double prevDistance;
  if (waterDistance > WATERTANK_HEIGHT)
    waterDistance = prevDistance;
  else
    prevDistance = waterDistance;
  double waterHeight = WATERTANK_HEIGHT - waterDistance;
  double waterVolume = waterHeight * WATERTANK_WIDTH * WATERTANK_DEPTH * CM3_TO_L;
  return waterVolume;
}

void showWaterLevelOnLCD(double waterLevel){
  lcd.setCursor(0,0);
  lcd.print("NivelH2O:");
  lcd.print(waterLevel);
  lcd.print(" L   ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void lightWaterLevelLeds(double waterLevel){
  if(waterLevel <= LEVEL_LOW){
    for(int i = 0; i < 3; i++)
      digitalWrite(LedArray[i],LedstateLow[i]);
  }
  else if (waterLevel <= LEVEL_MED){
    for(int i = 0; i < 3; i++)
      digitalWrite(LedArray[i],LedstateMed[i]);
  }
  else{
    for(int i = 0; i < 3; i++)
      digitalWrite(LedArray[i],LedstateHigh[i]);
  }
}

void overFlowWarning(unsigned long timeNow){
  lcd.setCursor(0,1);
  lcd.print("  !!Rebalse!!  ");
  if (timeNow - ovfTimerStart > ovfTimerDelay){
    ovfTimerStart += ovfTimerDelay;
    if(digitalRead(LED_LOW) == LOW){
      for(int i = 0; i < 3; i++)
        digitalWrite(LedArray[i],LedstateHigh[i]);
      digitalWrite(BUZZER_PIN,HIGH);
    }
    else{
      for(int i = 0; i < 3; i++)
        digitalWrite(LedArray[i],LedstateOvf[i]);
      digitalWrite(BUZZER_PIN,LOW);
    }
  }
}