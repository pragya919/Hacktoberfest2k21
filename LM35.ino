#include "ThingSpeak.h"
#include "WiFi.h"
#include "RTClib.h"
#include <Wire.h>

const int LM35 = 32;
int red = 18;
int green = 19;
int buzzer = 4;
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiClient  client;

unsigned long myChannelNumber = 1502782; // Channel ID here
const int FieldNumber = 3;
const char * myWriteAPIKey = "A7JAKNHHPSDYH6QF"; // Your Write API Key here

char ssid[] = "Gunjan"; //SSID
char pass[] = "abcdefgh"; // Password

void setup()
{ 
  Serial.begin(115200);
  Wire.begin();
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(buzzer, LOW);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
int analogAvgValues (int sensorPin)
{
 unsigned int total=0;
 float temp;
 for(int n=0; n<64; n++ )
 {
    total += analogRead (sensorPin);
    temp = (total/1024.0) * 1650.0;
    temp = (temp / 10.0);
 }
 return temp/64.0;
}

void loop()
{
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, LOW);
    DateTime now = rtc.now();
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.println(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.println();
//     calculate a date which is 7 days and 30 seconds into the future
//    DateTime future (now + TimeSpan(7,12,30,6));
//     
//    Serial.print(" Date after 7days and 30s: ");
//    Serial.print(future.year(), DEC);
//    Serial.print('/');
//    Serial.print(future.month(), DEC);
//    Serial.print('/');
//    Serial.print(future.day(), DEC);
//    Serial.print(' ');
//    Serial.print(future.hour(), DEC);
//    Serial.print(':');
//    Serial.print(future.minute(), DEC);
//    Serial.print(':');
//    Serial.print(future.second(), DEC);
//    Serial.println();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(3000);
    }
    Serial.println("\nConnected to the SSID.");
    Serial.println(ssid);
  }
  
  float temperature;
  const double temp_threshold = 44.0;
  const double buzzer_threshold = 79.0; 
  temperature = analogAvgValues(LM35);  /* LM35 gives output of 10mv/°C */
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println("*C,");
  float Far_temp;
  Far_temp = (temperature*9.0)/5.0 + 32.0;
  Serial.print(Far_temp);
  Serial.println("*F");
  Serial.println();
  
  if(temperature > temp_threshold && buzzer_threshold >= temperature)
  {
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(buzzer, LOW); 
  }
  else if(temperature > buzzer_threshold)
  { 
    digitalWrite(buzzer, HIGH);
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
  }
  else
  {
    digitalWrite(green,HIGH);
    digitalWrite(red, LOW);
    digitalWrite(buzzer, LOW);   
  }
  ThingSpeak.writeField(myChannelNumber, FieldNumber, temperature, myWriteAPIKey);
  delay(1000);
}
