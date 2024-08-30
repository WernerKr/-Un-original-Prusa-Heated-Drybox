/* 
Code for the (Un)original Prusa Drybox Heater
Written by Bram Elema, Creative Commons BY-NC-SA

Added support for a second temp/hum Sensor DHT21 or DHT22
and AutoOff Function 
and AutoHum Function
and new Fan control, and Led Strip control
If heater is set on, the data now updated each 5 sec otherwise 10 sec 
Werner Krenn

REQUIRES the following Arduino libraries:
 - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_SSD1306 Library: https://github.com/adafruit/Adafruit_SSD1306
 - DFRobot_DHT20 Sensor Library: https://github.com/DFRobot/DFRobot_DHT20
 - DHT_sensor_library Library: https://github.com/adafruit/DHT-sensor-library
*/

#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobot_DHT20.h>
#include <SPI.h>
#include <Wire.h>

// Uncomment the next line if you want the temperature to be displayed in Fahrenheit, it is displayed in Celcius by default
// #define Fahrenheit

// to see the Temp/Hum Data in the Serial Monitor uncomment next line
// #define debug
#define showFraction

#define Button1 6
#define Button2 7
#define Button3 8
#define Heater 10
#define Fan 5
#define Led 4

//#define PETG                         // Max = 70 °C
//#define ABS                          // Max = 83 °C
//#define ASA                          // Max = 93 °C
//#define PC                           // Max = 113 °C

// If an LED strip needs to be controlled 
#define controlLed
// if have attached a second temp/hum Sensor (dht22 or dht21) uncomment next line
#define SecondTemp
#define DHT_PIN 9 // The Arduino Nano pin connected to DHT21/22 sensor
#define DHT_TYPE DHT21
//#define DHT_TYPE DHT22 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET 4  
#define SCREEN_ADDRESS 0x3C  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DFRobot_DHT20 dht20;
DHT dht(DHT_PIN, DHT_TYPE);

bool overHeat = false;                     // control overheating - only used if second temp/hum is available
bool withDuct = true;                     // Duct installed -> Tempdiff = 0.2°C otherwise 0.5°C
int TargetTemp = 45;
int Max = 93;                             // overHeating Temp need #overHeat = true 
int MaxSet = 70;                          // max allowed setting TargetTemp
int MinSet = 20;                          // min allowed setting TargetTemp
int AutoOffTime = 360;                    // here 6 hours
int MaxAutoOffTime = 2880;                // max 2 days
int FanDelay = 300;                       // 300 seconds=5 minutes max 999 seconds because Display
int FanCor = 50;                          // >= 50°C TargetTemp add to Fandelay 120 sec
int FanCor1 = 55;
int FanCor2 = 60;
int FanCor3 = 65;
int FanCor4 = 70;
int FanCor01 = 40;
int FanCor02 = 35;
int FanCor03 = 30;
int FanCor04 = 25;

int TargetHum = 35;
int AutoHumValue = 35;
int MaxHumSet = 65;                        // max allowed setting TargetHum
int MinHumSet = 10;                        // min allowed setting TargetHum - but maybe will never be reached!
bool HumOff = false;

float TemperatureCor = 0.0;               // correction Temperatur
float HumidityCor = 0.0;                  // correction Humidity
float Temperature2Cor = 0.0;              // correction Temperatur2 if #define SecondTemp 
float Humidity2Cor = 0.0;                 // correction Humidity2 if #define SecondTemp

unsigned long previousMillis = 0;
unsigned long AutoOffpreviousMillis = 0;
unsigned long AutoOffSetpreviousMillis = 0;
unsigned long FanOnpreviousMillis = 0;
unsigned long FanHumOnpreviousMillis = 0;
unsigned long debugMillis = 0;

int FanValue = 0;
int AutoOffTimeValue;
bool status = false;
bool Hot = false;
bool AutoOff = false;
bool AutoOffSet = false;
bool AutoHum = false;
bool AutoHumSet = false;
bool FanOn = false;
bool FanHumOn = false;
bool TempHigh = false;
bool showSecondTemp = false;
bool showLed = false;
char text[] = "°C";

float Temperature;
float Humidity;
float Temperature2;
float Humidity2;
float tempDiff;

char TempIntegerDisplay[4];
char TempFractionDisplay[4];
char HumIntegerDisplay[4];
char HumFractionDisplay[4];
char TargetTempDisplay[4];
char TargetHumDisplay[2];
char MinuteDisplay[4];


void setup(){
  Serial.begin(9600);
  //Serial.begin(115200);                             

  // Initialize buttons and heater
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  pinMode(Heater, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(Led, OUTPUT);

  #ifdef SecondTemp
   dht.begin(); // initialize the sensor
   showSecondTemp = true;
  #endif
  #ifdef controlLed
   showLed = true;
   //digitalWrite(Led, LOW);
  #endif

  // Check if sensor and display are working
  if(dht20.begin()){
    Serial.println("Initialize sensor failed");
    delay(1000);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Initialize display failed");
    delay(1000);
  }

  if (withDuct == true)
  {
   tempDiff = 0.2;
  } else {
   tempDiff = 0.5;
  }
  #ifndef Fahrenheit 
   #ifdef PC
     int Max = 113;
   #endif
   #ifdef ASA
     int Max = 93;
   #endif
   #ifdef ABS
     int Max = 83;
   #endif
   #ifdef PETG
     int Max = 70;
   #endif
  #endif 
  #ifdef Fahrenheit
    TargetTemp = 113;
    MaxSet = 158;
    int Max = 200;
    #ifdef PC
     int Max = 235;
    #endif
    #ifdef ASA
     int Max = 200;
    #endif
    #ifdef ABS
     int Max = 181;
    #endif
    #ifdef PETG
     int Max = 158;
    #endif

    MinSet = 50;
    int FanCor = 122;
    int FanCor1 = 131;
    int FanCor2 = 140;
    int FanCor3 = 149;
    int FanCor4 = 158;
    int FanCor01 = 104;
    int FanCor02 = 95;
    int FanCor03 = 86;
    int FanCor04 = 77;

    char text[] = "°F"; 
   if (withDuct == true)
    {
     tempDiff = 0.5;
    } else {
     tempDiff = 1.0;
    } 
  #endif

  AutoOffTimeValue = AutoOffTime;
  FanValue = FanDelay;

  drawLogo();
  delay(2000); 
  sensorUpdate();
}

void loop(){
  // Heating system ON:
  while(status == true){
    unsigned long currentMillis = millis();
    FanOnpreviousMillis = currentMillis;

  if (AutoOff == true) {
     if(currentMillis - AutoOffpreviousMillis >= 60000) {     // AutoOff Time reached (check every 1 Minutes)
      AutoOffpreviousMillis = currentMillis;
      AutoOffTimeValue = AutoOffTimeValue - 1;
      if (AutoOffTimeValue <= 0){
      status = false;
      digitalWrite(Heater, LOW);
      }
     }
  }   

    if (AutoHum == true) {
      unsigned long currentMillisHum = millis();
      if (Humidity <= AutoHumValue - 0.5)
       { 
        digitalWrite(Heater, LOW); 
        HumOff = true;
        Hot = false;
        FanHumOn = true;
       }
      if ((Humidity >= AutoHumValue + 1.0))
       { 
        if (TempHigh == false)
        {
         if(Temperature < (TargetTemp - tempDiff))
         {
          digitalWrite(Fan, HIGH);
          //digitalWrite(Heater, HIGH);
          Hot = true;
         } 
        } 
        HumOff = false;
        FanHumOn = false;
       }
       if (HumOff == false) {FanHumOnpreviousMillis = currentMillisHum; }
       if (FanHumOn == true) {FanValue = FanDelay - ((currentMillisHum - FanHumOnpreviousMillis)/1000);}
       if (( FanHumOn == true) and ((currentMillisHum - FanHumOnpreviousMillis)/1000 >= FanDelay)) {     // Switch off Fan delay
       FanHumOn = false;
       digitalWrite(Fan, LOW);
       }
    }

    if((digitalRead(Button2) == LOW) and (digitalRead(Button1) == LOW))
    {  
     AutoOffSet = true;
     AutoHum = false;
     AutoHumSet = false;
     AutoOffSetpreviousMillis = currentMillis;
     AutoOff = true;
    }

    if((digitalRead(Button3) == LOW) and (digitalRead(Button1) == LOW))
    {  
     AutoOffSet = false;
     AutoHum = true;
     AutoHumSet = true;
     AutoOffSetpreviousMillis = currentMillis;
     AutoOff = false;
    }

    if ((AutoOffSet == true) and (currentMillis - AutoOffSetpreviousMillis >= 30000) )       // after 30 seconds settings for AutoOffSet off
    {
     AutoOffSet = false;
    }

    if ((AutoHumSet == true) and (currentMillis - AutoOffSetpreviousMillis >= 30000) )       // after 30 seconds settings for AutoHumSet off
    {
     AutoHumSet = false;
    }
 

    if ((digitalRead(Button1) == LOW) and (digitalRead(Button2) == HIGH) and (digitalRead(Button3) == HIGH) ){                 // Action button 1: switch heating system from on to off
      status = !status;
      AutoOff = false;
      AutoHum = false;
      AutoOffSet = false;
      AutoHumSet = false;
      AutoOffTimeValue = AutoOffTime;
      if (FanOn == true)
      {FanOnpreviousMillis = currentMillis;}
      sensorUpdate();
      digitalWrite(Heater, LOW);
      Hot = false;
      display.clearDisplay();
      drawStatus();
      if (status == true) {
       digitalWrite(Fan, HIGH);
       FanOn = true;
      }
      if (not showSecondTemp){
       drawTemperature();
       drawHumidity();
      }
      else
      {
       drawTemperature1();
       drawHumidity1();
       drawTemperature2();
       drawHumidity2();
      }
      display.display(); 
      delay(200);
    }

    if(digitalRead(Button2) == LOW) {                 // Action button 2: increase target temperature
     if ((AutoOffSet == false) and (AutoHumSet == false)) {
      TargetTemp = TargetTemp + 1;
      if (TargetTemp > MaxSet)
      { TargetTemp = MaxSet;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawTargetTemperature();
      if (TargetTemp >= FanCor04) { FanDelay = 120; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor03) { FanDelay = 180; FanValue = FanDelay;}
      if (TargetTemp >= FanCor02) { FanDelay = 240; FanValue = FanDelay;}
      if (TargetTemp >= FanCor01) { FanDelay = 300; FanValue = FanDelay;}
      if (TargetTemp >= FanCor) { FanDelay = 420; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor1) { FanDelay = 540; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor2) { FanDelay = 660; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor3) { FanDelay = 780; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor4) { FanDelay = 900; FanValue = FanDelay;} 
     } 
     if ((AutoOffSet == true) and (AutoHumSet == false))                          // or increase AutoOff time
     {                                          
      AutoOffTimeValue = AutoOffTimeValue + 10;
      if (AutoOffTimeValue > MaxAutoOffTime)
      { AutoOffTimeValue = MaxAutoOffTime; }
      previousMillis = currentMillis;
      display.clearDisplay();
      drawAutoOfftMinutes();
     }
     if ((AutoHumSet == true) and (AutoOffSet == false))
     {
      AutoHumValue = AutoHumValue + 1;
      if (AutoHumValue > MaxHumSet)
      { AutoHumValue = MaxHumSet;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawAutoHum();
     }

     display.display();
     delay(200);
    }

    if(digitalRead(Button3) == LOW) {                 // Action button 3: decrease target temperature
     if ((AutoOffSet == false) and (AutoHumSet == false)) {
       TargetTemp = TargetTemp - 1;
       if (TargetTemp < MinSet)
        { TargetTemp = MinSet;}
      previousMillis = currentMillis;
      display.clearDisplay();
      drawTargetTemperature();
      if (TargetTemp < FanCor4) { FanDelay = 780; FanValue = FanDelay;}
      if (TargetTemp < FanCor3) { FanDelay = 660; FanValue = FanDelay;}
      if (TargetTemp < FanCor2) { FanDelay = 540; FanValue = FanDelay;}
      if (TargetTemp < FanCor1) { FanDelay = 420; FanValue = FanDelay;}
      if (TargetTemp < FanCor) { FanDelay = 300; FanValue = FanDelay;}
      if (TargetTemp < FanCor01) { FanDelay = 240; FanValue = FanDelay;}
      if (TargetTemp < FanCor02) { FanDelay = 180; FanValue = FanDelay;}
      if (TargetTemp < FanCor03) { FanDelay = 120; FanValue = FanDelay;}
      if (TargetTemp < FanCor04) { FanDelay = 60; FanValue = FanDelay;}
     } 
     if (AutoOffSet == true)
      {
       AutoOffTimeValue = AutoOffTimeValue - 10;
       if (AutoOffTimeValue <= 0 )
       { AutoOffTimeValue = 10; }
       previousMillis = currentMillis;
       display.clearDisplay();
       drawAutoOfftMinutes();
     }
     if ((AutoHumSet == true) and (AutoOffSet == false))
     {
       AutoHumValue = AutoHumValue - 1;
       if (AutoHumValue < MinHumSet)
       { AutoHumValue = MinHumSet;}      
       previousMillis = currentMillis;
       display.clearDisplay();
       drawAutoHum();
     }

     display.display();
     delay(200);
    }

    if(currentMillis - previousMillis >= 5000) {     // Refreshes data on the display (every 5 seconds)
      previousMillis = currentMillis;
      sensorUpdate();

      display.clearDisplay();
      drawStatus();
      if (not showSecondTemp){
       drawTemperature();
       drawHumidity();
      }
      else
      {
       drawTemperature1();
       drawHumidity1();
       drawTemperature2();
       drawHumidity2();
      }
      display.display();    
      heater();
    }
    if (overHeat == true)
    {
      if ((TempHigh == false) and (Hot == true))
      {
        digitalWrite(Heater, HIGH); 
      }
    }

  }

  // Heating system OFF:
  while(status == false){
    unsigned long currentMillis = millis();
    AutoOff = false;
    AutoOffSet = false;
    AutoOffTimeValue = AutoOffTime;
    digitalWrite(Heater, LOW);
    
    if (FanOn == true)
    {   
    FanValue = FanDelay - ((currentMillis - FanOnpreviousMillis)/1000);  
    if((currentMillis - FanOnpreviousMillis)/1000 >= FanDelay) {     // Switch off Fan delay
      FanOn = false;
      digitalWrite(Fan, LOW);
      }
    }

    if(digitalRead(Button1) == LOW) {                 // Action button 1: switch heating system from off to on
      status = !status;
      sensorUpdate();
      if (status == true) {
       digitalWrite(Fan, HIGH);
       FanOn = true;
      }
      display.clearDisplay();
      drawStatus();
      if (not showSecondTemp){
       drawTemperature();
       drawHumidity();
      }
      else
      {
       drawTemperature1();
       drawHumidity1();
       drawTemperature2();
       drawHumidity2();
      }
      display.display(); 
      delay(200);
    }
    #ifdef SecondTemp
    if (status == false){
     if((digitalRead(Button3) == LOW) and (not showSecondTemp)) {                 // Action button 3: second Temp view on
      showSecondTemp = true;
      display.clearDisplay();
      drawStatus();
      drawTemperature1();
      drawHumidity1();
      drawTemperature2();
      drawHumidity2();
      display.display(); 
      delay(200);
     } 
     if((digitalRead(Button3) == LOW) and (showSecondTemp)) {                 // Action button 3: second Temp view off
      showSecondTemp = false;
      display.clearDisplay();
      drawStatus();
      drawTemperature();
      drawHumidity();
      display.display(); 
      delay(200);
     } 
    }
    #endif

    #ifdef controlLed
    if (status == false){
     if((digitalRead(Button2) == LOW) and (not showLed)) {                 // Action button 2: LED on
      showLed = true;
      digitalWrite(Led, HIGH);
      /*
      display.clearDisplay();
      drawStatus();
      drawTemperature1();
      drawHumidity1();
      drawTemperature2();
      drawHumidity2();
      display.display(); 
      */
      delay(200);
     } 
     if((digitalRead(Button2) == LOW) and (showLed)) {                 // Action button 2: LED off
      showLed = false;
      digitalWrite(Led, LOW);
      /*
      display.clearDisplay();
      drawStatus();
      drawTemperature();
      drawHumidity();
      display.display(); 
      */
      delay(200);
     } 
    }
    #endif

    if(currentMillis - previousMillis >= 10000) {     // Refreshes data on the display (every 10 seconds)
      previousMillis = currentMillis;
      sensorUpdate();

      display.clearDisplay();
      drawStatus();
       if (not showSecondTemp){
       drawTemperature();
       drawHumidity();
      }
      else
      {
       drawTemperature1();
       drawHumidity1();
       drawTemperature2();
       drawHumidity2();
      }
      display.display();
    }

    #ifdef debug
    if(currentMillis - debugMillis >= 2000) {     // Updates Serial Monitor data every 2 seconds
      debugMillis = currentMillis;
    Serial.print("DHT20: Humidity: ");
    Serial.print(Humidity);
    Serial.print("%");
    Serial.print("  |  "); 
    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.println(text);

    #ifdef SecondTemp
     Serial.print("DHT21/22: Humidity: ");
     Serial.print(Humidity2);
     Serial.print("%");
     Serial.print("  "); 
     Serial.print("Temperature: ");
     Serial.print(Temperature2);
     Serial.println(text);
    #endif
    }
    #endif
  }

}