#include <WiFi.h>
#include <ESP32Ping.h>
#include "time.h"
#include <LiquidCrystal.h>
LiquidCrystal My_LCD(13, 12, 14, 27, 26, 25);
#define yellow 15
#define blue 4
#define green 2
#define red 16
#define buz 17

///////wifi credentials////////////////////

const char* ssid     = "MRINAL";
const char* password = "mrinal maity";

const IPAddress remote_ip(8, 8, 8, 8);
const char* remote_host = "www.google.com";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

///////////function declaration///////////////

void connectingWifi();
void printLocalTime();
void ipCheck();
void processingSig();
void onlineStatus();
void offlineStatus();
void wifiSignalQuality();
void allOff();
void pingTest();

/////////////////////////////////////////////////

int singalQuality[]={100,100,100,100,100,100,100,100,100,100,100,100,100,100,
100,100,100,100,100,100,100,99,99,99,98,98,98,97,97,96,96,95,95,94,93,93,92,
91,90,90,89,88,87,86,85,84,83,82,81,80,79,78,76,75,74,73,71,70,69,67,66,64,
63,61,60,58,56,55,53,51,50,48,46,44,42,40,38,36,34,32,30,28,26,24,22,20,
17,15,13,10,8,6,3,1,1,1,1,1,1,1,1};

int flag = 0;
int flag2 = 0;
int wifiRSSI = 0;
float pingTime = 0;

const int ledfreq = 5000;
const int buzfreq = 1000;
const int leddutyCycle = 80;
const int buzdutyCycle = 0;

const int redChannel = 3;
const int greenChannel = 2;
const int blueChannel = 1;
const int yellowChannel = 0;
const int buzChannel = 4;
const int resolution = 8;

void setup() 
{
  pinMode(yellow, OUTPUT);//yellow
  pinMode(blue, OUTPUT);//green
  pinMode(green, OUTPUT);//blue
  pinMode(red, OUTPUT);//red
  pinMode(buz, OUTPUT);//buzzer
  
  ledcSetup(redChannel, ledfreq, resolution);
  ledcSetup(greenChannel, ledfreq, resolution);
  ledcSetup(blueChannel, ledfreq, resolution);
  ledcSetup(yellowChannel, ledfreq, resolution);
  ledcSetup(buzChannel, buzfreq, resolution);

  ledcAttachPin(green, greenChannel);
  ledcAttachPin(red, redChannel);
  ledcAttachPin(blue, blueChannel);
  ledcAttachPin(yellow, yellowChannel);
  ledcAttachPin(buz, buzChannel);
  
  ledcWrite(buzChannel, 250);
  delay(1000);
  ledcWrite(buzChannel, 0);
  for(int i = 0; i<=3; i++)
  {
    ledcWrite(i, 250);
    delay(250);
    ledcWrite(i, 0);
    delay(250);
  }
  
  My_LCD.begin(20, 4);
  My_LCD.clear();
  My_LCD.setCursor(3,1);
  My_LCD.print("ESP PING-MASTER");
  My_LCD.setCursor(5,2);
  My_LCD.print("by @MRINAL");
  delay(3000);
  connectingWifi();
  
}

void loop() 
{
  if(WiFi.status() == WL_CONNECTED)
  {
    My_LCD.clear();
    wifiSignalQuality();
    printLocalTime();
    ipCheck();
    pingTest();
    printLocalTime();
    delay(3000);
    allOff();
  }
  else
  {
    connectingWifi();
  }
}

void connectingWifi()
{
    My_LCD.clear();
    ledcWrite(yellowChannel, 0);
    My_LCD.print("Connecting to WiFi");
    WiFi.begin(ssid, password); 
    while (WiFi.status() != WL_CONNECTED) 
    {
      ledcWrite(blueChannel, leddutyCycle);
      delay(250);
      ledcWrite(blueChannel, 0);
      delay(250);
    }
    ledcWrite(yellowChannel, leddutyCycle);
    My_LCD.setCursor(5,2);
    My_LCD.print("Connected!");
    delay(1000);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    My_LCD.setCursor(0,2);
    My_LCD.print("Time Failed");
    return;
  }
  My_LCD.setCursor(0,2);
  My_LCD.print(&timeinfo, "%b %d %Y %H:%M:%S");
}

void ipCheck()
{
  My_LCD.setCursor(0,0);
  My_LCD.print("IP: ");
  My_LCD.setCursor(4,0);
  My_LCD.print(WiFi.localIP());
  My_LCD.setCursor(0,1);
  My_LCD.print("Ping: ");
  My_LCD.setCursor(6,1);
  My_LCD.print(remote_host);
}


void processingSig()
{
  if(flag == 0)
  {
    ledcWrite(blueChannel, leddutyCycle);
  }
}

void onlineStatus()
{
      if(flag2 == 1)
      {
        My_LCD.setCursor(0,3);
        My_LCD.print("Online MS=");
        My_LCD.setCursor(10,3);
        My_LCD.print(pingTime,0);
      }
}

void offlineStatus()
{
      if(flag2 == 2)
      {
        My_LCD.setCursor(0,3);
        My_LCD.print("No internet");
      }   
}

void wifiSignalQuality()
{
      wifiRSSI = WiFi.RSSI()*(-1);
      My_LCD.setCursor(16,3);
      My_LCD.print(singalQuality[wifiRSSI]);
      My_LCD.setCursor(19,3);
      My_LCD.print("%");
}

void allOff()
{
    ledcWrite(greenChannel, 0);
    ledcWrite(redChannel, 0);
    ledcWrite(buzChannel, 0);
    flag = 0;
}

void pingTest()
{
    processingSig();
    onlineStatus();
    offlineStatus();
    if(Ping.ping(remote_host)) //if(Ping.ping(remote_ip)) 
    {
      pingTime = Ping.averageTime();    
      flag = flag + 1;
      flag2 = 1;
      ledcWrite(blueChannel, 0);
      ledcWrite(greenChannel, leddutyCycle);
    }
    else
    {  
      flag = flag + 1;
      flag2 = 2;
      ledcWrite(blueChannel, 0);
      ledcWrite(redChannel, leddutyCycle);
      ledcWrite(buzChannel, buzdutyCycle);
    }  
}
