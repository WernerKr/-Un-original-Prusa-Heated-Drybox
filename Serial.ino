/* 25
Code for the (Un)original Prusa Drybox Heater
Written by Bram Elema, Creative Commons BY-NC-SA

Added support for a second temp/hum Sensor DHT21 or DHT22
and AutoOff Function 
and AutoHum Function
and new Fan control, and Led Strip control
If heater is set on, the data now updated each 5 sec otherwise 10 sec 
The TargetTemp is now 50°C, the AutoHum ist now 25%
Update Display if Heating on is now 2 sec, tempDiff is now 0.1°C/0.18°F
settings are now in arduino_settings.h
Werner Krenn - last modified 2025-02-01

REQUIRES the following Arduino libraries:
 - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_SSD1306 Library: https://github.com/adafruit/Adafruit_SSD1306
 - DFRobot_DHT20 Sensor Library: https://github.com/DFRobot/DFRobot_DHT20
 - DHT_sensor_library Library: https://github.com/adafruit/DHT-sensor-library
*/

#include <Arduino.h>
#include "arduino_settings.h"

#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobot_DHT20.h>
#include <SPI.h>
#include <Wire.h>

#define Button1 6                       // on/off
#define Button2 7                       // + increase
#define Button3 8                       // - decrease  
#define Heater 10
#define Fan 5
#define Led 4

//#define PETG                         // Max = 73 °C	163
//#define ABS                          // Max = 83 °C	181
//#define ASA                          // Max = 93 °C	199
//#define PC                           // Max = 123 °C	253
//#define PA                           // Max = 143 °C	289

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
int AutoHumValue = 25; 
bool AutoHum = false;
bool AutoHumSet = false;
bool HeatMaxSet = false;
bool FanOn = false;
bool FanHumOn = false;
bool TempHigh = false;
bool showSecondTemp = false;
bool showLed = false;
bool showLedSet = false;
char text[] = "°C";

float Temperature;
float Humidity;
float Temperature2;
float Humidity2;
float tempDiff;
bool HumOff = false;                       // AutoHum trigger 

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
   tempDiff = 0.1;
  } else {
   tempDiff = 0.2;
  }

  #ifndef Fahrenheit
   int HeatMaxValue= Max;
  #endif 

  #ifdef Fahrenheit
    TargetTemp = int((TargetTemp *9/5) + 32);
    MaxSet = int((MaxSet *9/5) + 32);
    int Max = int((Max *9/5) + 32);
    int HeatMax = int((HeatMax *9/5) + 32);
    int HeatMin = int((HeatMin *9/5) + 32);

    int HeatMaxValue= Max;
    MinSet = int((MinSet *9/5) + 32);
    int FanCor = int((FanCor *9/5) + 32);
    int FanCor1 = int((FanCor1 *9/5) + 32);
    int FanCor2 = int((FanCor2 *9/5) + 32);
    //int FanCor3 = int((FanCor3 *9/5) + 32);
    //int FanCor4 = int((FanCor4 *9/5) + 32);
    int FanCor01 = int((FanCor01 *9/5) + 32);
    int FanCor02 = int((FanCor02 *9/5) + 32);
    int FanCor03 = int((FanCor03 *9/5) + 32);
    int FanCor04 = int((FanCor04  *9/5) + 32);

    char text[] = "°F"; 
   if (withDuct == true)
    {
     tempDiff = 0.18;
    } else {
     tempDiff = 0.36;
    } 
  #endif

  AutoHumValue = TargetHum;
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
      if (Humidity <= AutoHumValue - humDiff)
       { 
        digitalWrite(Heater, LOW); 
        HumOff = true;
        Hot = false;
        FanHumOn = true;
       }
      if ((Humidity >= AutoHumValue + humDiff))
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
      if ( (Hot == false) and (FanValue <=0)){
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

    #ifdef SecondTemp 
    if((digitalRead(Button3) == LOW) and (digitalRead(Button2) == LOW))
    { 
     HeatMaxSet = true;  
     AutoOffSetpreviousMillis = currentMillis;
    }
    #endif

    if ((AutoOffSet == true) and (currentMillis - AutoOffSetpreviousMillis >= 30000) )       // after 30 seconds settings for AutoOffSet off
    {
     AutoOffSet = false;
    }

    if ((AutoHumSet == true) and (currentMillis - AutoOffSetpreviousMillis >= 30000) )       // after 30 seconds settings for AutoHumSet off
    {
     AutoHumSet = false;
    }

    #ifdef SecondTemp 
    if ((HeatMaxSet == true) and (currentMillis - AutoOffSetpreviousMillis >= 30000) )       // after 30 seconds settings for AutoHumSet off
    {
     HeatMaxSet = false;
    }
    
    #ifdef controlLed
    #ifdef ControllLedAutooff

    #ifdef Fahrenheit
    if ((showLed == true) and  (Temperature2 >= 140)) {
    showLedSet = true;
    digitalWrite(Led, LOW);
    } 
    #endif
    #ifndef Fahrenheit
    if ((showLed == true) and  (Temperature2 >= 60)) {
    showLedSet = true;
    digitalWrite(Led, LOW);
    } 
    #endif

    #endif 
    #endif
    
    #endif

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
      HeatMaxValue = Max;
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
     if ((AutoOffSet == false) and (AutoHumSet == false) and (HeatMaxSet == false)) {
      TargetTemp = TargetTemp + 1;
      if (TargetTemp > MaxSet)
      { TargetTemp = MaxSet;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawTargetTemperature();
      if (TargetTemp >= FanCor04) { FanDelay = 60; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor03) { FanDelay = 120; FanValue = FanDelay;}
      if (TargetTemp >= FanCor02) { FanDelay = 180; FanValue = FanDelay;}
      if (TargetTemp >= FanCor01) { FanDelay = 240; FanValue = FanDelay;}
      if (TargetTemp >= FanCor) { FanDelay = 300; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor1) { FanDelay = 420; FanValue = FanDelay;} 
      if (TargetTemp >= FanCor2) { FanDelay = 570; FanValue = FanDelay;} 
      //if (TargetTemp >= FanCor3) { FanDelay = 780; FanValue = FanDelay;} 
      //if (TargetTemp >= FanCor4) { FanDelay = 900; FanValue = FanDelay;} 
     } 
     if ((AutoOffSet == true) and (AutoHumSet == false) and (HeatMaxSet == false))                          // or increase AutoOff time
     {                                          
      AutoOffTimeValue = AutoOffTimeValue + 10;
      if (AutoOffTimeValue > MaxAutoOffTime)
      { AutoOffTimeValue = MaxAutoOffTime; }
      previousMillis = currentMillis;
      display.clearDisplay();
      drawAutoOfftMinutes();
     }
     if ((AutoHumSet == true) and (AutoOffSet == false) and (HeatMaxSet == false))
     {
      AutoHumValue = AutoHumValue + 1;
      if (AutoHumValue > MaxHumSet)
      { AutoHumValue = MaxHumSet;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawAutoHum();
     }
    #ifdef SecondTemp      
     if ((AutoHumSet == false) and (AutoOffSet == false) and (HeatMaxSet == true))
     {
      HeatMaxValue = HeatMaxValue + 1;
      if (HeatMaxValue > HeatMax)
      { HeatMaxValue = HeatMax;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawHeatMax();
     }
    #endif

     display.display();
     delay(200);
    }

    if(digitalRead(Button3) == LOW) {                 // Action button 3: decrease target temperature
     if ((AutoOffSet == false) and (AutoHumSet == false) and (HeatMaxSet == false)) {
       TargetTemp = TargetTemp - 1;
       if (TargetTemp < MinSet)
        { TargetTemp = MinSet;}
      previousMillis = currentMillis;
      display.clearDisplay();
      drawTargetTemperature();
      //if (TargetTemp < FanCor4) { FanDelay = 780; FanValue = FanDelay;}
      //if (TargetTemp < FanCor3) { FanDelay = 660; FanValue = FanDelay;}
      if (TargetTemp < FanCor2) { FanDelay = 420; FanValue = FanDelay;}
      if (TargetTemp < FanCor1) { FanDelay = 300; FanValue = FanDelay;}
      if (TargetTemp < FanCor) { FanDelay = 240; FanValue = FanDelay;}
      if (TargetTemp < FanCor01) { FanDelay = 180; FanValue = FanDelay;}
      if (TargetTemp < FanCor02) { FanDelay = 120; FanValue = FanDelay;}
      if (TargetTemp < FanCor03) { FanDelay = 60; FanValue = FanDelay;}
      if (TargetTemp < FanCor04) { FanDelay = 30; FanValue = FanDelay;}
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
     if ((AutoHumSet == true) and (AutoOffSet == false) and (HeatMaxSet == false))
     {
       AutoHumValue = AutoHumValue - 1;
       if (AutoHumValue < MinHumSet)
       { AutoHumValue = MinHumSet;} 
       if (AutoHumValue < 23)
       {
        #ifdef Fahrenheit
         if (TargetTemp < 131) {
            TargetTemp = 131;
            FanDelay = 420; 
            FanValue = FanDelay;
         } 
        #endif
        #ifndef Fahrenheit
         if (TargetTemp < 55) {
            TargetTemp = 55;
            FanDelay = 420; 
            FanValue = FanDelay;
         } 
        #endif

       }
       previousMillis = currentMillis;
       display.clearDisplay();
       drawAutoHum();
     }
    #ifdef SecondTemp      
     if ((AutoHumSet == false) and (AutoOffSet == false) and (HeatMaxSet == true))
     {
      HeatMaxValue = HeatMaxValue - 1;
      if (HeatMaxValue < HeatMin)
      { HeatMaxValue = HeatMin;}      
      previousMillis = currentMillis;
      display.clearDisplay();
      drawHeatMax();
     }
    #endif

     display.display();
     delay(200);
    }

    if(currentMillis - previousMillis >= Refresh) {     // Refreshes data on the display (every x mseconds)
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
      delay(200);
     } 
     if((digitalRead(Button2) == LOW) and (showLed)) {                 // Action button 2: LED off
      showLed = false;
      showLedSet = false;
      digitalWrite(Led, LOW);
      delay(200);
     } 
    }

    #ifdef SecondTemp
    #ifdef ControllLedAutooff
    #ifdef Fahrenheit
    if (((showLed == true) and (showLedSet == true)) and (Temperature2 < 140)) {
    showLedSet = false;
    digitalWrite(Led, HIGH);
    } else {
      if (Temperature2 < 140) {showLedSet == false;}
      }
    #endif
    #ifndef Fahrenheit
    if (((showLed == true) and (showLedSet == true)) and (Temperature2 < 60)) {
    showLedSet = false;
    digitalWrite(Led, HIGH);
    } else {
      if (Temperature2 < 60) {showLedSet == false;}
      }
    #endif
    #endif 
    #endif

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
    if ((Humidity >0))
    {   
    Serial.print(Humidity);
    Serial.print("%");
    Serial.print("  |  "); 
    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.println(text);
    }
    else {Serial.println("Temperature: "); }

    #ifdef SecondTemp
     Serial.print("DHT21/22: Humidity: ");
     if ((Humidity2 >0))
     {
     Serial.print(Humidity2);
     Serial.print("%");
     Serial.print("  "); 
     Serial.print("Temperature: ");
     Serial.print(Temperature2);
     Serial.println(text);
     }
     else {Serial.println("Temperature: "); }
    #endif
    }
    #endif
  }

}