/*
FastLED example code comes from the FastLED library. I am afraid I do not know who deserves that credit.

  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


// LED initialization
#include <FastLED.h>
using namespace fl;
// How many leds in your strip?
#define NUM_LEDS 392 
// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 4
#define CLOCK_PIN 13
// Define the array of leds
CRGB leds[NUM_LEDS];

#include <EEPROM.h>
#define EEPROM_SIZE 64

// Wifi and time initialization
#include <WiFi.h>
#include "time.h"
const char* ssid     = "UseYourNetworkNameHere";
const char* password = "UseYourPasswordHere";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
String colorString;
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Global variables and constants
int secondCounter = 0;
bool secondTriggered = false;
int sixtiethCounter = 0;
bool sixtiethTriggered = false;
int minuteCounter = 0;
int hourCounter = 0;
int previousHourCounter = 0;
int previousMinuteCounter = 0;
bool hourRollover = false;
bool minuteRollover = false;
long flashTimeout = 0;
int flashLength = 300;
int sixtiethColor = 64;
int secondColor = 200;
int minuteColor = 255;
int hourColor = 200;
byte timeZone = 0;
const int timeZoneEEPROMAddress = 0;
int expiredMinuteColor = 100;
int currentMode = 1;
const int modeEEPROMAddress = 2;
int LEDBrightness = 1;
int colorScheme = 0;

/*
  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.
  created 9 May 2011
  by Tom Igoe
  This example code is in the public domain.
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/SerialEvent
*/

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


int colorCounter = 0;

const byte quadrantCount = 4;
const byte quadrantSize = 98;
const byte ringCount = 10;
const byte ringSize[ringCount] = {1, 3, 5, 7, 9, 10, 13, 15, 16, 19};
const int ringAddress[ringCount][19] = {
  {0},
  {1, 2, 3},
  {4, 5, 6, 7, 8},
  {9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23, 24},
  {25, 26, 27, 28, 29, 30, 31, 32, 33, 34},
  {35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
  {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62},
  {63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78},
  {79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97}
};



void setup(){
  // Serial setup
	Serial.begin(115200);
  Serial.print(F("\n\n\nWelcome to Vibe Clock\n"));
  printHelpMenu();
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // FastLED setup
	FastLED.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
	FastLED.setBrightness(LEDBrightness);
  clearLEDs();
  
  circleBurst();
  
  if (!EEPROM.begin(EEPROM_SIZE)){
    Serial.println(F("failed to initialise EEPROM"));
  }
  timeZone = EEPROM.read(timeZoneEEPROMAddress);
  if (timeZone > 24){
    timeZone = 0;
  }
  currentMode = EEPROM.read(modeEEPROMAddress);
  
  Serial.print(F("Time Zone: "));
  Serial.print(timeZone);
  Serial.print("\n");
  
 // Wi-Fi setup
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  // Print local IP address and start web server
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  clearLEDs();
}

void loop() {
  gatherSerialData();
  currentModeSwitch();
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client){
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Do something with the incoming data
            if(header.indexOf("GET /mode") >= 0) {
              currentMode++;
              if(currentMode > 3){
                currentMode = 0;
                clearLEDs();
              }
              EEPROM.write(modeEEPROMAddress, currentMode);
              EEPROM.commit();
            }else if(header.indexOf("GET /brightness") >= 0) {
              LEDBrightness = LEDBrightness + 16;
              if(LEDBrightness == 256){
                LEDBrightness = 255;
              }
              if(LEDBrightness > 256){
                LEDBrightness = 1;
              }
              FastLED.setBrightness(LEDBrightness);
              Serial.print("Brightness:\t");
              Serial.print(LEDBrightness);
              Serial.print("\n");
            }else if(header.indexOf("GET /timezone") >= 0) {
              timeZone++;
              if(timeZone > 23){
                timeZone = 0;
              }
              EEPROM.write(timeZoneEEPROMAddress, timeZone);
              EEPROM.commit();
              Serial.print("Time zone:\t");
              Serial.print(timeZone);
              Serial.print("\n");
            }else if(header.indexOf("GET /scheme") >= 0) {
              colorScheme++;
              if(colorScheme > 3){
                colorScheme = 0;
              }
              if(colorScheme == 0){
                secondColor = 200;
                minuteColor = 255;
                expiredMinuteColor = 100;
                hourColor = 65;
              }else if(colorScheme == 1){
                secondColor = 200;
                minuteColor = 231;
                expiredMinuteColor = 135;
                hourColor = 65;
              }else if(colorScheme == 2){
                secondColor = 200;
                minuteColor = 88;
                expiredMinuteColor = 75;
                hourColor = 65;
              }else if(colorScheme == 3){
                secondColor = 200;
                minuteColor = 160;
                expiredMinuteColor = 220;
                hourColor = 65;
              }
            }else if(header.indexOf("GET /prandom") >= 0) {
              secondColor = random(0, 255);
              minuteColor = random(0, 255);
              hourColor = random(0, 255);
              expiredMinuteColor = random(0, 255);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Vibe Clock Settings</h1>");
            
            // Display the current mode 
            String currentModeString = String(currentMode);
            client.println("<p>Bezel " + currentModeString + "</p>");
            // Show the "mode" button
            client.println("<p><a href=\"/mode\"><button class=\"button\">Bezel</button></a></p>");
            
            // Display the current brightness 
            String brightnessString = String(LEDBrightness/16);
            client.println("<p><br />LED Level " + brightnessString + " / 15</p>");
            // Show the "Brightness" button
            client.println("<p><a href=\"/brightness\"><button class=\"button\">Brightness</button></a></p>");
            
            // Change between pre-made color schemes
            String colorSchemeString = String(colorScheme);
            client.println("<p><br />Color Scheme " + colorSchemeString + "</p>");
            // Show the "Colors" button
            client.println("<p><a href=\"/scheme\"><button class=\"button\">Colors</button></a></p>");
            
            // Randomize the colors
            client.println("<p><br />Pseudo-randomize colors</p>");
            // Show the "PRandom" button
            client.println("<p><a href=\"/prandom\"><button class=\"button\">PRandom</button></a></p>");
            
            // Display the current brightness 
            String timeZoneString = String(timeZone);
            client.println("<p><br />Time zone " + timeZoneString + " / 23</p>");
            // Show the "Time Zone" button
            client.println("<p><a href=\"/timezone\"><button class=\"button\">Time Zone</button></a></p>");
            
               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
