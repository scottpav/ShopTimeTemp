#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_7segment tempDisplay = Adafruit_7segment();

const char* ssid2 = "ATT3Y3V8j4";
const char* password2 = "327d796p5c6n";
const char* ssid = "Bridge";
const char* password = "Faith1212";

const char* host = "128.138.141.172";
int hours = 0;                      // Track hours
int minutes = 0;                    // Track minutes
int seconds = 0;                    // Track seconds
int tzOffset = -5;                  // Time zone offset 
bool blinkColon = false;            // Track the status of the colon to blink every second


#define ONE_WIRE_BUS 14 //D1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void setup() {
  Serial.begin(115200);                   // diagnostic channel
  matrix.begin(0x70);
  tempDisplay.begin(0x72);
  delay(10);
  Serial.println();
  Serial.println();
 Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
delay(30000); // I give it 30 sec to connect, if it fails, try next

if (WiFi.status() != WL_CONNECTED) {
  Serial.print("Connecting to ");
Serial.println(ssid2);
WiFi.begin(ssid2, password2);
delay(30000); // I give it 30 sec to connect, if it fails.....
}

else if (WiFi.status() != WL_CONNECTED) {
Serial.print("Going DeepSleep");
ESP.deepSleep(1000000); // go deepsleep for 10 min and try all over again
}

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() 
{
  if ((minutes == 0) || ((minutes % 5) == 0)) {
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 13;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }  
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMcu Lua;)\r\n\r\n");
  delay(100);
  char buffer[12];
  while(client.available())
    {
      String line = client.readStringUntil('\r');
        hours =  line.substring(16, 18).toInt();
        hours += tzOffset;
        if (hours < 0) {
          hours = 24 + hours;
        }
        if (hours > 23) {
          hours = hours - 23;
        }
      minutes = line.substring(19, 21).toInt();
    }
  }
  int timeNow = hours*100 + minutes;
  if (hours > 12) {
    timeNow -= 1200;
  }
  else if (hours == 0) {
    timeNow += 12;
  }
  matrix.print(timeNow);
  blinkColon = !blinkColon;
  matrix.drawColon(blinkColon);
  matrix.writeDisplay();
  printTemp();
  delay(1000);
    seconds += 1;
    // If the seconds go above 59 then the minutes should increase and
    // the seconds should wrap back to 0.
    if (seconds > 59) {
      seconds = 0;
      minutes += 1;
      // Again if the minutes go above 59 then the hour should increase and
      // the minutes should wrap back to 0.
      if (minutes > 59) {
        minutes = 0;
        Serial.println("Minutes set to zero - should query NTP on next loop()");
        hours += 1; 
        if (hours > 23) {
          hours = 0;
        }
      }
    }
}

float getTemperatureF() {
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempFByIndex(0);
    delay(100);
  } 
  while (temp == 85.0 || temp == (-127.0));
  return temp;
}

float getTemperature() {
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } 
  while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void printTemp(){
    float currentTemperature = getTemperatureF();
    int temp = int(round(currentTemperature));
    tempDisplay.print(currentTemperature);
    tempDisplay.writeDigitRaw(4, 0x71);
    tempDisplay.writeDisplay();
}



