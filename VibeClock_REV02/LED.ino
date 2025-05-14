void updateClockFace(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char funCharSeconds[3];
  strftime(funCharSeconds,3, "%S", &timeinfo);
  secondCounter = atoi(funCharSeconds);
  char funCharMinutes[3];
  strftime(funCharMinutes,3, "%M", &timeinfo);
  minuteCounter = atoi(funCharMinutes);
  char funCharHours[3];
  strftime(funCharHours,3, "%H", &timeinfo);
  hourCounter = atoi(funCharHours);
  
  if(previousHourCounter != hourCounter){
    hourRollover = true;
    flashTimeout = millis() + flashLength;
  }
  previousHourCounter = hourCounter;
  if((previousMinuteCounter != minuteCounter)){
    minuteRollover = true;
    flashTimeout = millis() + flashLength;
  }
  previousMinuteCounter = minuteCounter;
  
  if(millis() > flashTimeout){
    hourRollover = false;
    minuteRollover = false;
  }
  
  if((currentMode == 0)){
    // No bezel activity
    fillCircles();
    minuteFill();
    hourHand();
    if((millis()%10000 == 0)){
      clearBezel();
    }
  }
  if(currentMode == 1){
    // Flash the bezel every hour
    fillCircles();
    minuteFill();
    hourHand();
    if(hourRollover){
      innerBezel(65);
      outerBezel(65);
    }else{
      clearBezel();
    }
  }
  if(currentMode == 2){
    // Flash the bezel every minute
    fillCircles();
    minuteFill();
    hourHand();
    if(minuteRollover){
      innerBezel(65);
      outerBezel(65);
    }else{
      clearBezel();
    }
  }
  if(currentMode == 3){
    // Toggle the bezel rings every second
    fillCircles();
    minuteFill();
    hourHand();
    clearBezel();
    if((millis()%2000 < 1000)){
      innerBezel(65);
    }else{
      outerBezel(65);
    }
  }
  FastLED.show();
}

void clearBezel(){
  // i is the quadrant
  // k is the inner and outer rings
  // n is the ring in question
  for(int i=0;i<4;i++){
    for(int k=8;k<10;k++){
      for(int n=0;n<ringSize[k];n++){
        int funLEDAddress = ringAddressLocation(k, n);
        leds[funLEDAddress + i*98] = CHSV(0, 0, 0);
      }
    }
  }
}

void innerBezel(int funColor){
  // i is the quadrant
  // k is the inner and outer rings
  // n is the ring in question
  for(int i=0;i<4;i++){
    for(int k=8;k<9;k++){
      for(int n=0;n<ringSize[k];n++){
        int funLEDAddress = ringAddressLocation(k, n);
        leds[funLEDAddress + i*98] = CHSV(funColor, 255, 255);
      }
    }
  }
}

void outerBezel(int funColor){
  // i is the quadrant
  // k is the inner and outer rings
  // n is the ring in question
  for(int i=0;i<4;i++){
    for(int k=9;k<10;k++){
      for(int n=0;n<ringSize[k];n++){
        int funLEDAddress = ringAddressLocation(k, n);
        leds[funLEDAddress + i*98] = CHSV(funColor, 255, 255);
      }
    }
  }
}

void hourHand(){
  struct tm timeinfo;
  char funCharHours[3];
  strftime(funCharHours,3, "%H", &timeinfo);
  hourCounter = atoi(funCharHours);
  hourCounter = hourCounter + timeZone;
  while(hourCounter>=12){
    hourCounter = hourCounter - 12;
  }
  while(hourCounter<0){
    hourCounter = hourCounter + 12;
  }
  
  if((hourCounter == 0) || (hourCounter == 3) || (hourCounter == 6) || (hourCounter == 9) || (hourCounter == 12)){
    int funTimeMultiplier = hourCounter - 0;
    funTimeMultiplier = funTimeMultiplier/3;
    funTimeMultiplier = funTimeMultiplier * 98;
    leds[0 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[1 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[4 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[9 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[16 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    
  }
  if((hourCounter == 1) || (hourCounter == 4) || (hourCounter == 7) || (hourCounter == 10)){
    int funTimeMultiplier = hourCounter - 1;
    funTimeMultiplier = funTimeMultiplier/3;
    funTimeMultiplier = funTimeMultiplier * 98;
    leds[0 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[11 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[18 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[19 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
  }
  if((hourCounter == 2) || (hourCounter == 5) || (hourCounter == 8) || (hourCounter == 11)){
    int funTimeMultiplier = hourCounter - 2;
    funTimeMultiplier = funTimeMultiplier/3;
    funTimeMultiplier = funTimeMultiplier * 98;
    leds[0 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[13 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[21 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
    leds[22 + funTimeMultiplier] = CHSV(hourColor, 255, 255);
  }
}

void minuteFill(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char funCharMinutes[3];
  strftime(funCharMinutes,3, "%M", &timeinfo);
  minuteCounter = atoi(funCharMinutes);
  
  for(int i=0;i<=7;i++){
    float funMinuteFloat = (ringSize[i] / 15.0) * minuteCounter;
    int funMinuteInt = funMinuteFloat;
    int funLEDAddress = ringAddressLocation(i, funMinuteInt);
    for(int k=0; k<=funMinuteInt; k++){
      funLEDAddress = ringAddressLocation(i, k);
      leds[funLEDAddress] = CHSV(expiredMinuteColor, 255, 255);
    }
  }
}



void clearLEDs(){
  for (int i=0; i < NUM_LEDS; i++){
    leds[i] = CHSV(0, 0, 0);
  }
  FastLED.show();
}

void currentModeSwitch(){
  switch(currentMode){
    case 0:
      updateClockFace();
      break;
    case 1:
      updateClockFace();
      break;
    case 2:
      updateClockFace();
      break;
    case 3:
      updateClockFace();
      break;
    default:
      break;
  }
}

void circleBurst(){
  for(int i=0;i<ringCount;i++){
    for(int k=0;k<ringSize[i];k++){
      for(int n=0;n<quadrantCount;n++){
        leds[ringAddress[i][k] + quadrantSize*n] = CHSV(colorCounter, 255, 255);
      }
    }
    incrementColor(255/10);
    FastLED.show();
  }
}

void incrementColor(int funIncrement){
  if(funIncrement == 0){
    funIncrement = 1;
  }
  colorCounter = colorCounter + funIncrement;
  if(colorCounter>255){
    colorCounter = 0;
  }
}


void OBSOLETE01secondsLine(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  char funCharSeconds[3];
  strftime(funCharSeconds,3, "%S", &timeinfo);
  secondCounter = atoi(funCharSeconds);
  int funIntSeconds00 = (funCharSeconds[0] - '0');
  int funIntSeconds01 = (funCharSeconds[1] - '0');
  int funIntSeconds = funIntSeconds00*10 + funIntSeconds01;
  Serial.print("Seconds: ");
  Serial.print(funIntSeconds);
  Serial.print("\n");
  
}

void secondsLine(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println(F("Failed to obtain time"));
    return;
  }
  char funCharSeconds[3];
  strftime(funCharSeconds,3, "%S", &timeinfo);
  secondCounter = atoi(funCharSeconds);
  
  int funLEDAddress = ringAddressLocation(7, secondCounter);
  int funPreviousLED = ringAddressLocation(7, secondCounter-1);
  leds[funPreviousLED] = CHSV(secondColor, 0, 0);
  leds[funLEDAddress] = CHSV(secondColor, 255, 255);
  FastLED.show();
}

void fillCircles(){
  for(int i=0;i<4;i++){
    for(int k=0;k<63;k++){
      leds[k + i*98] = CHSV(minuteColor, 255, 255);
    }
  }
}

int ringAddressLocation(int funRing, int funRotation){
  int funReturnAddress = 999;
  if((funRing < 0) || (funRing > ringCount) || (funRotation < -1) || (funRotation > 19*4)){
    return funReturnAddress;
  }
  if(funRotation == -1){
    funReturnAddress = ringAddress[funRing][ringSize[funRing] - 1] + quadrantSize*3;
    return funReturnAddress;
  }
  int funQuadrant = funRotation / ringSize[funRing];
  if(funQuadrant >= quadrantCount){
    return 999;
  }
  funReturnAddress = ringAddress[funRing][funRotation - (ringSize[funRing] * funQuadrant)] + (98 * funQuadrant);
  return funReturnAddress;
}

void secondsFill(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println(F("Failed to obtain time"));
    return;
  }
//  char funCharMinutes[3];
//  strftime(funCharMinutes,3, "%M", &timeinfo);
//  minuteCounter = atoi(funCharMinutes);
  char funCharSeconds[3];
  strftime(funCharSeconds,3, "%S", &timeinfo);
  secondCounter = atoi(funCharSeconds);
  char funCharHours[3];
  strftime(funCharHours,3, "%H", &timeinfo);
  hourCounter = atoi(funCharHours);
  if(previousHourCounter != hourCounter){
    // Use this to update the hour hand position
    Serial.print("Hour: ");
    Serial.print(hourCounter);
    Serial.print("\n");
  }
  previousHourCounter = hourCounter;

  if(secondCounter == 0){
    fillCircles();
  }
  
  for(int i=0;i<=7;i++){
    float funSecondFloat = (ringSize[i] / 15.0) * secondCounter;
    int funSecondInt = funSecondFloat;
    
    int funLEDAddress = ringAddressLocation(i, funSecondFloat);
    int funPreviousLED = ringAddressLocation(i, funSecondFloat-1);
//    leds[funPreviousLED] = CHSV(expiredMinuteColor, 255, 255);
    leds[funLEDAddress] = CHSV(expiredMinuteColor, 255, 255);
  }
  FastLED.show();
}
