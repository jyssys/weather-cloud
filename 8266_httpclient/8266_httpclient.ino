#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

 
const char* ssid     = "SK_WiFiC8CE";      // SSID of local network
const char* password = "1306002316";   // Password on network
String APIKEY = "9c266b2444993166a17415060ceed746";
String CityID = "1835848"; // Seoul, KR
//String CityID = "5128581"; // New York, NA
 
WiFiClient client;
char servername[]="api.openweathermap.org";
String result;
 
int  counter = 60;
 
String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
 
void setup() {
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
}

void loop() {
    if(counter == 60) //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delay(5000);
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
 
weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
 
}
 
void displayWeather(String location,String description)
{
   Serial.println(location);
   Serial.println(description);
}
 
void displayConditions(float Temperature,float Humidity, float Pressure)
{
  Serial.println(Temperature);

  Serial.println(Humidity);

  Serial.println(Pressure); 
}
 
void displayGettingData()
{
  
}
