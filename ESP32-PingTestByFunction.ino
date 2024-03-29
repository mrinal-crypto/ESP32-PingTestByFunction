#include <WiFi.h>
#include <ESP32Ping.h>
#include <WiFiManager.h>
#include "time.h"
#include <LiquidCrystal.h>
LiquidCrystal My_LCD(13, 12, 14, 27, 26, 25);
#define yellow 15
#define blue 4
#define green 2
#define red 16
#define buz 17


const IPAddress remote_ip(8, 8, 8, 8);
const char* remote_host = "www.google.com";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

///////////function declaration///////////////

void connectingWifi();
void printLocalTime();
void ipCheck();
void remoteHost();
void udLink();
void processingSig();
void internetStatus();
void wifiSignalQuality();
void allOff();
void pingTest();

//////////////custom char//////////////////////

//byte sig[] = {
//  B00000,
//  B00000,
//  B11100,
//  B00010,
//  B11001,
//  B00101,
//  B10101,
//  B00000
//};

byte upload[] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};
byte download[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

//////////////////////////////////////////////

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
//const int buzfreq = 1000;
const int leddutyCycle = 15; //brightness
const int gLedOffset = 7;
//const int buzdutyCycle = 0;

const int redChannel = 3;
const int greenChannel = 2;
const int blueChannel = 1;
const int yellowChannel = 0;
//const int buzChannel = 4;
const int resolution = 8;



void connectingWifi(int cWx, int cWy)
{

    WiFiManager wm;    
     

  
    My_LCD.clear();
    ledcWrite(yellowChannel, 0);
    My_LCD.setCursor(cWx, cWy);
    My_LCD.print("Connecting to WiFi");
    WiFi.disconnect();
    delay(50);
    bool success = false;
    
    while(!success) {
        ledcWrite(blueChannel, leddutyCycle);
        
        My_LCD.setCursor(cWx, cWy + 2);
        My_LCD.print("   PING-MASTER   ");
        My_LCD.setCursor(cWx, cWy + 3);
        My_LCD.print("SetupIP-192.168.4.1");
        
        wm.setConfigPortalTimeout(60);
        success = wm.autoConnect("ESP PING-MASTER");    
        if(!success){
          ledcWrite(blueChannel, 0);
          My_LCD.setCursor(cWx, cWy + 1);
          My_LCD.print("Failed!");
        }      
    } 


    String ssid = WiFi.SSID();
    
    ledcWrite(yellowChannel, leddutyCycle);
    My_LCD.setCursor(cWx, cWy + 1);
    My_LCD.print("Connected");
    My_LCD.setCursor(cWx, cWy + 2);
    My_LCD.print("                ");
    My_LCD.setCursor(cWx, cWy + 2);
    My_LCD.print(ssid);
    My_LCD.setCursor(cWx, cWy + 3);
    My_LCD.print("                   ");

    ipCheck(1, 3);
    delay(5000);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(50);
}

void printLocalTime(int pLTx, int pLTy)
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    My_LCD.setCursor(pLTx, pLTy);
    My_LCD.print("Time Failed");
  }
  My_LCD.setCursor(pLTx, pLTy);
  My_LCD.print(&timeinfo, "  %H:%M  %d.%b.%y"); //Y=2022, %S=sec
}

void ipCheck(int iCx, int iCy)
{
  My_LCD.setCursor(iCx, iCy);
  My_LCD.print("IP: ");
  My_LCD.setCursor(iCx + 4, iCy);
  My_LCD.print(WiFi.localIP());
}

void remoteHost(int rHx, int rHy)
{
  My_LCD.setCursor(rHx, rHy);
  My_LCD.print("Ping: ");
  My_LCD.print(remote_host);  //My_LCD.print(remote_host);
//  My_LCD.print(" -t");
}

void processingSig(int pS)
{
  if(pS == 1)
  {
    ledcWrite(blueChannel, leddutyCycle);  
  }
  if(pS == 0)
  {
    ledcWrite(blueChannel, 0);
  }
}

void udLink(int udx, int udy, int ud){
  if(ud == 1){
    My_LCD.setCursor(udx, udy);
    My_LCD.write(2);
    My_LCD.setCursor(udx + 1, udy);
    My_LCD.print(" ");
  }
  if(ud == 2){
    My_LCD.setCursor(udx, udy);
    My_LCD.print(" ");
    My_LCD.setCursor(udx + 1, udy);
    My_LCD.write(3);
  }
}

void internetStatus(int iSx, int iSy, int iS)
{
      if(iS == 1)
      {
        ledcWrite(greenChannel, leddutyCycle-gLedOffset);
        ledcWrite(redChannel, 0);
        My_LCD.setCursor(iSx, iSy);
        My_LCD.print("Online ");
        //My_LCD.setCursor(iSx + 10, iSy);
        My_LCD.print(pingTime, 0);
        My_LCD.print(" ms");
      }
      if(iS == 2)
      {
        ledcWrite(redChannel, leddutyCycle);
        ledcWrite(greenChannel, 0);
        My_LCD.setCursor(iSx, iSy);
        My_LCD.print("No internet");
      }
}

void wifiSignalQuality(int wSQx, int wSQy)
{
      wifiRSSI = WiFi.RSSI()*(-1);
      My_LCD.setCursor(wSQx, wSQy);
      //My_LCD.write(1);
      My_LCD.print(singalQuality[wifiRSSI]);
      My_LCD.print("%");
}

void allOff()
{
    ledcWrite(greenChannel, 0);
    ledcWrite(redChannel, 0);
//    ledcWrite(buzChannel, 0);
}

void pingTest()
{
    flag = 1;
    processingSig(flag);
    internetStatus(0, 3, flag2);
    if(Ping.ping(remote_host)) //if(Ping.ping(remote_ip)) remote_host
    {
      pingTime = Ping.averageTime();    
      flag = 0;
      flag2 = 1;
      processingSig(flag);
      ledcWrite(greenChannel, leddutyCycle-gLedOffset);
    }
    else
    {  
      flag = 0;
      flag2 = 2;
      processingSig(flag);
      ledcWrite(redChannel, leddutyCycle);
//      ledcWrite(buzChannel, buzdutyCycle);
    }  
}

void setup() 
{

//  Serial.begin(115200);

//  delay(50);
  
  pinMode(yellow, OUTPUT);//yellow
  pinMode(blue, OUTPUT);//green
  pinMode(green, OUTPUT);//blue
  pinMode(red, OUTPUT);//red
  pinMode(buz, OUTPUT);//buzzer

//  My_LCD.createChar(1, sig);
  My_LCD.createChar(2, upload);
  My_LCD.createChar(3, download);
  
  ledcSetup(redChannel, ledfreq, resolution);
  ledcSetup(greenChannel, ledfreq, resolution);
  ledcSetup(blueChannel, ledfreq, resolution);
  ledcSetup(yellowChannel, ledfreq, resolution);
//  ledcSetup(buzChannel, buzfreq, resolution);

  ledcAttachPin(green, greenChannel);
  ledcAttachPin(red, redChannel);
  ledcAttachPin(blue, blueChannel);
  ledcAttachPin(yellow, yellowChannel);
//  ledcAttachPin(buz, buzChannel);
//
//  delay(50);
//  
  digitalWrite(buz, HIGH);
  delay(2000);
  digitalWrite(buz, LOW);

//  delay(50);
//  for(int i = 0; i<=3; i++)
//  {
//    ledcWrite(i, 250);
//    delay(200);
//    ledcWrite(i, 0);
//    delay(200);
//  }
  
  delay(50);
  My_LCD.begin(20, 4);
  My_LCD.clear();
  My_LCD.setCursor(3,1);
  My_LCD.print("ESP PING-MASTER");
  My_LCD.setCursor(6,2);
  My_LCD.print("by MRINAL");
  delay(5000);
  
  connectingWifi(1, 0);
  
}

void loop() 
{
  if(WiFi.status() == WL_CONNECTED)
  {
    My_LCD.clear();
    wifiSignalQuality(16, 3);
    ipCheck(0, 0);
    udLink(18, 0, 1);
    printLocalTime(0, 2);
    remoteHost(0, 1);
    pingTest();
    printLocalTime(0, 2);
    udLink(18, 0, 2);
    delay(3000);
    allOff();
  }
  else
  {
    connectingWifi(1, 0);
  }
}
