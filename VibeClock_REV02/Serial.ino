
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent(){
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("\n");
  Serial.print(F("ISO Format date\t"));
  Serial.print(&timeinfo, "%Y-%m-%d");
  Serial.print("\n");
  Serial.print(F("ISO Format time\tT"));
  Serial.print(&timeinfo, "%H:%M:%S%z");
  Serial.print("\n");
  //http://www.cplusplus.com/reference/ctime/strftime/
}


void gatherSerialData(){
  if (stringComplete) {
    // Remove the newline or carriage return from inputString
    inputString.trim();
    String firstEleven = inputString.substring(0,11);
    String firstTen = inputString.substring(0,10);
    String firstNine = inputString.substring(0,9);
    String colorNumerals = inputString.substring(11,15);
    String brightnessNumerals = inputString.substring(10,13);
    String timeZoneNumerals = inputString.substring(9,12);
    if(firstEleven == F("secondColor") || firstEleven == F("second Colo") || firstEleven == F("Second Colo") || firstEleven == F("secondcolor") || firstEleven == F("second colo")){
      
      secondColor = colorNumerals.toInt();
      Serial.print(F("Second color:\t"));
      Serial.print(secondColor);
      Serial.print("\n");
    }else if(firstEleven == F("minuteColor") || firstEleven == F("minute Colo") || firstEleven == F("Minute Colo") || firstEleven == F("minutecolor") || firstEleven == F("minute colo")){
      
      minuteColor = colorNumerals.toInt();
      Serial.print(F("Minute color:\t"));
      Serial.print(minuteColor);
      Serial.print("\n");
    }else if(firstEleven == F("expMinColor") || firstEleven == F("expMin Colo") || firstEleven == F("ExpMin Colo") || firstEleven == F("expmincolor") || firstEleven == F("expmin colo")){
      
      expiredMinuteColor = colorNumerals.toInt();
      Serial.print(F("Expired minute color:\t"));
      Serial.print(expiredMinuteColor);
      Serial.print("\n");
    }else if(firstEleven == F("hrhandcolor") || firstEleven == F("hour hand c") || firstEleven == F("Hour hand c") || firstEleven == F("Hour Hand C") || firstEleven == F("HRHandColor") || firstEleven == F("HrHandColor") || firstEleven == F("HrhandColor") || firstEleven == F("HRHandcolor") || firstEleven == F("HrHandColor")){
      
      hourColor = colorNumerals.toInt();
      Serial.print(F("Hour hand color:\t"));
      Serial.print(hourColor);
      Serial.print("\n");
    }else if(firstTen == F("Brightness") || firstTen == F("brightness")){
      LEDBrightness = brightnessNumerals.toInt();
      FastLED.setBrightness(LEDBrightness);
    }else if(inputString == F("Color") || inputString == F("color")){
      Serial.print(F("Second hand color    :\t"));
      Serial.print(secondColor);
      Serial.print(F("\nMinute hand color    :\t"));
      Serial.print(minuteColor);
      Serial.print(F("\nExpired minutes color:\t"));
      Serial.print(expiredMinuteColor);
      Serial.print(F("\nHour color           :\t"));
      Serial.print(hourColor);
      Serial.print("\n");
    }else if(inputString == F("ResetColor") || inputString == F("Reset Color") || inputString == F("resetcolor") || inputString == F("reset color")){
      secondColor = 200;
      minuteColor = 255;
      expiredMinuteColor = 100;
      hourColor = 65;
    }else if(inputString == F("m0") || inputString == F("M0") || inputString == F("mode 0") || inputString == F("Mode 0")){
      currentMode = 0;
      Serial.print(F("Mode 0 - no bezel activity.\n"));
      EEPROM.write(modeEEPROMAddress, currentMode);
      EEPROM.commit();
    }else if(inputString == F("m1") || inputString == F("M1") || inputString == F("mode 1") || inputString == F("Mode 1")){
      currentMode = 1;
      Serial.print(F("Mode 1 - flash the bezel every hour.\n"));
      EEPROM.write(modeEEPROMAddress, currentMode);
      EEPROM.commit();
    }else if(inputString == F("m2") || inputString == F("M2") || inputString == F("mode 2") || inputString == F("Mode 2")){
      currentMode = 2;
      Serial.print(F("Mode 2 - flash the bezel every minute.\n"));
      EEPROM.write(modeEEPROMAddress, currentMode);
      EEPROM.commit();
    }else if(inputString == F("m3") || inputString == F("M3") || inputString == F("mode 3") || inputString == F("Mode 3")){
      currentMode = 3;
      Serial.print(F("Mode 3 - toggle the bezel rings every second.\n"));
      EEPROM.write(modeEEPROMAddress, currentMode);
      EEPROM.commit();
    }else if(inputString == F("mode") || inputString == F("Mode") || inputString == F("MODE")){
      Serial.print(F("Mode "));
      Serial.print(currentMode);
      Serial.print("\n");
      EEPROM.write(modeEEPROMAddress, currentMode);
      EEPROM.commit();
    }else if(inputString == F("h") || inputString == F("H") || inputString == F("?")){
      printHelpMenu();
    }else if(inputString == F("time") || inputString == F("Time") || inputString == F("TIME")){
      printLocalTime();
    }else if(firstNine == F("Time zone") || firstNine == F("Time Zone") || firstNine == F("time zone") || firstNine == F("time Zone") || firstNine == F("TIME ZONE")){
      timeZone = timeZoneNumerals.toInt();
      EEPROM.write(timeZoneEEPROMAddress, timeZone);
      EEPROM.commit();
      Serial.print(F("Time Zone: "));
      byte funTempTimeZone = EEPROM.read(timeZoneEEPROMAddress);
      Serial.print(funTempTimeZone);
      Serial.print("\n");
    }else{
      Serial.print(F("!!!!!!!!!!!!!!\nUnrecognized:\t"));
      Serial.print(inputString);
      printHelpMenu();
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void printHelpMenu(){
  Serial.print("\n");
  Serial.print(F("?          \tShow help menu\n"));
  Serial.print(F("Time       \tShow current time\n"));
  Serial.print(F("SecondColor\tFollow with a number, no spaces, 0-255 to change the second hand color\n"));
  Serial.print(F("MinuteColor\tFollow with a number, no spaces, 0-255 to change the minutes color\n"));
  Serial.print(F("ExpMinColor\tFollow with a number, no spaces, 0-255 to change the expired minutes color\n"));
  Serial.print(F("hrhandcolor\tFollow with a number, no spaces, 0-255 to change the hour hand minutes color\n"));
  Serial.print(F("Reset color\tUse the default colors\n"));
  Serial.print(F("Brightness \tFollow with a number, 0-255, to set the brightness\n"));
  Serial.print(F("Time Zone  \tFollow with a number, 0-23, to set the time zone correction\n"));
  Serial.print(F("Color      \tPrint the colors in use\n"));
  Serial.print(F("Mode 0     \tMode 0: Turn off the LEDs\n"));
  Serial.print(F("Mode 1     \tMode 1: Wipe blue to red\n"));
  Serial.print(F("Mode 2     \tMode 2: Wipe off to red\n"));
  Serial.print(F("Mode 3     \tMode 3: Wipe green to off\n"));
  Serial.print("\n");
}
