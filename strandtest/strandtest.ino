#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>

#endif

/*****************************************************************************
Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

*****************************************************************************/
WiFiClient client;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

uint8_t dataPin  = D3;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = D4;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(20, dataPin, clockPin);


/* Innitializing api communication*/
const char* ssid     = "SK_WiFiC8CE";      // SSID of local network
const char* password = "1306002316";   // Password on network

//const char* ssid     = "AndroidHotspot2775";      // SSID of local network
//const char* password = "12345678";   // Password on network

String APIKEY = "9c266b2444993166a17415060ceed746";
//String CityID = "1835848"; // Seoul, KR
String CityID = "5128581"; // New York, NA
//String CityID = "6173331"; // Vancouver, CA
//String CityID = "5983607"; //Inuvik, CA

char servername[]="api.openweathermap.org";
String result;
 
int  counter = 60;
 
String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float WeatherId;
/*------------------------------*/
void setup() {
   lcd.init();
  lcd.backlight();
  
  Serial.begin(115200);
  int cursorPosition=0;
  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    cursorPosition++;
  }
  Serial.println("Connected");
  delay(1000);
  
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
}


void loop() {
  // Some example procedures showing how to display to the pixels
  /*---------------------------------------------*/

  if(counter == 60) //Get new data every 10 minutes
    {
      counter = 0;
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delay(50);
      displayConditions(Temperature,Humidity,Pressure);
      delay(50);
      WeatherId = 771;
      displayId(WeatherId);
    }
}

void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }
 
  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }
 
  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);
 
char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;

JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}
 
String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];

//float weatherId = root["weather"]["id"];
 
weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
//WeatherId = weatherId;
 
}
 
void displayWeather(String location,String description)
{
   Serial.println(location);
   lcd.setCursor(0,0);
   lcd.print(location);
   Serial.println(description);
}
 
void displayConditions(float Temperature,float Humidity, float Pressure)
{
  Serial.println(Temperature);
  lcd.setCursor(0,1);
  lcd.print("Temp : ");
  lcd.print(Temperature);
  lcd.print("Cel");

  Serial.println(Humidity);

  Serial.println(Pressure); 
}

void displayId(float weatherId)
{
  if((weatherId > 199 && weatherId < 233) || weatherId == 771 || weatherId == 781){
    //폭풍
    storm();
  }
  else if(weatherId > 799 && weatherId < 805){
    //맑음 && 구름의 양
    sunny();
  }
  else if(weatherId > 299 && weatherId < 532){
    //비
    rain();
  }
  else if(weatherId > 599 && weatherId < 623){
    //눈
    snow();
    snow2();
  }
  else if(weatherId > 700 && weatherId < 763){
    //미세먼지
    fineDust();
  }
  else;
}

void storm()
{
  for (int j=0; j<10; j++) { 
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Color(255,255,255)); 
      }
      strip.show();
     
      delay(200);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0); 
      }
    }
  }
}

void sunny()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Color(136,206,250));
      strip.show();
      delay(100);
  }
}

void rain()
{
  for(uint16_t i=0; i<strip.numPixels(); i+=3) {
      strip.setPixelColor(i, Color(176,196,222));
      strip.show();
      delay(100);
  }
}

void snow()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i == 3 || i == 7 || i == 11 || i == 15 || i == 19)
    {
      strip.setPixelColor(i, Color(255,255,222));
    }
    else
    {
      strip.setPixelColor(i, Color(0,0,0));
    }
      strip.show();
  }
  delay(400);
}
void snow2()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i == 0 || i == 4 || i == 8 || i == 12 || i == 16)
    {
      strip.setPixelColor(i, Color(255,255,222));
    }
    else
    {
      strip.setPixelColor(i, Color(0,0,0));
    }
      strip.show();
  }
  delay(400);

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i == 1 || i == 5 || i == 9 || i == 13 || i == 17)
    {
      strip.setPixelColor(i, Color(255,255,222));
    }
    else
    {
      strip.setPixelColor(i, Color(0,0,0));
    }
      strip.show();
  }
  delay(400);

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i == 2 || i == 6 || i == 10 || i == 14 || i == 18)
    {
      strip.setPixelColor(i, Color(255,255,222));
    }
    else
    {
      strip.setPixelColor(i, Color(0,0,0));
    }
      strip.show();
  }
  delay(400);
}


void fineDust()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Color(244,164,20));
      strip.show();
      delay(100);
  }
}

/*-----------------------------------------------*/
void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void club(uint8_t wait) {
  int i, j,even, odd;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
      for (i=0; i < strip.numPixels(); i += 2) {
        strip.setPixelColor(i, Color(102,232,33));
      }  // write all the pixels out
      strip.show();
      delay(wait);

      for (i=0; i < strip.numPixels(); i += 2) {
        strip.setPixelColor(i, 0);
      }  // write all the pixels out
      strip.show();
      delay(wait);

      for (i=1; i < strip.numPixels(); i += 2) {
        strip.setPixelColor(i, Color(255,255,255));
      }  // write all the pixels out
      strip.show();
      delay(wait);

      for (i=1; i < strip.numPixels(); i += 2) {
        strip.setPixelColor(i, 0);
      }  // write all the pixels out
      strip.show();
      delay(wait);
    /*strip.setPixelColor(0, Wheel( (i + j) % 255));
    strip.show();
    for ( i=1; i < strip.numPixels(); i += 2) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  // write all the pixels out
    strip.show();
    delay(wait);*/
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) { 
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c); 
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0); 
      }
    }
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
