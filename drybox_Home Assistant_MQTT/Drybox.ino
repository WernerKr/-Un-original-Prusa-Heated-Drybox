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
Werner Krenn - last modified 2025-01-06

REQUIRES the following Arduino libraries:
 - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_SSD1306 Library: https://github.com/adafruit/Adafruit_SSD1306
 - DFRobot_DHT20 Sensor Library: https://github.com/DFRobot/DFRobot_DHT20
 - DHT_sensor_library Library: https://github.com/adafruit/DHT-sensor-library
 HA MQTT:
 - ESP32 core v3.0.3
 - ArduinoJson https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 - MQTTRemote 
 - HomeAssistantEntities https://github.com/Johboh/HomeAssistantEntities
 - 
*/
#include <Arduino.h>
// This is for the Arduino IDE, where we always build with ArduinoJson. arduino-cli will not build/include libraries
// that are not included anywhere. So we must include ArduinoJson.h so its available for IJson.h later.
// For Platform IO, this is not the case and these examples are built both with ArduinoJson and nlohmann-json.
#ifndef PLATFORMIO
#include <ArduinoJson.h>
#endif
#include <HaBridge.h>
// This is here for CI only. You can remove this include and include ArduinoJson.h or nlohmann/json.hpp directly.
#include <IJson.h>
#include <MQTTRemote.h>

#include <entities/HaEntitySwitch.h>
#include <entities/HaEntityNumber.h>
//#include <entities/HaEntityText.h>
#include <entities/HaEntityTemperature.h>
#include <entities/HaEntityHumidity.h>
#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error "Unsupported hardware. Sorry!"
#endif

#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobot_DHT20.h>
#include <SPI.h>
#include <Wire.h>

#include "arduino_settings.h"

const char wifi_ssid[] = WIFI_SSID;    	// your network SSID (name)
const char wifi_password[] = WIFI_PASS;    	// your network password (use for WPA, or use as key for WEP)
const char mqtt_client_id[] = MQTT_ID;
const char mqtt_host[] = MQTT_HOST;
const char mqtt_username[] = MQTT_USER;
const char mqtt_password[] = MQTT_PASS;

// Uncomment the next line if you want the temperature to be displayed in Fahrenheit, it is displayed in Celcius by default
// #define Fahrenheit

// to see the Temp/Hum Data in the Serial Monitor uncomment next line
// #define debug

#define showFraction                    // all values with decimal values otherwise only integer values

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

// If an LED strip needs to be controlled otherwise uncomment next line
#define controlLed
#define ControllLedAutooff            // Led on and Temperature >=60°C/140°F (= max. operating temperature LED strip) the led is switched off

// if have not attached a second temp/hum Sensor (dht22 or dht21) uncomment next line
#define SecondTemp                    // should be placed on the floor under the heater or duct

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

bool overHeat = true;                     // control overheating - only used if second temp/hum is available
bool withDuct = true;                     // Duct installed -> Tempdiff = 0.1°C otherwise 0.3°C
int TargetTemp = 50;
int Refresh = 2000;                       // Update values in msec if heater is on (former 5000)
int MaxSet = 70;                          // max allowed setting TargetTemp °C
int MinSet = 20;                          // min allowed setting TargetTemp °C
int AutoOffTime = 360;                    // here 6 hours
int MaxAutoOffTime = 2880;                // max 2 days
int FanDelay = 300;                       // 300 seconds=5 minutes max 999 seconds because space Display
int FanCor = 50;                          // >= 50°C TargetTemp add to Fandelay 60 or 90 sec
int FanCor1 = 55;                         // all FanCor in °C - are later converted to Fahrenheit if set
int FanCor2 = 60;
//int FanCor3 = 65;
//int FanCor4 = 70;
int FanCor01 = 40;
int FanCor02 = 35;
int FanCor03 = 30;
int FanCor04 = 25;

int TargetHum = 25;
int AutoHumValue = 25;
int MaxHumSet = 65;                        // max allowed setting TargetHum
int MinHumSet = 10;
float humDiff = 0.1;                       // diff between hum values on/off
bool HumOff = false;                       // AutoHum trigger 

int Max = 85;                             // overHeating Temp under the duct, need #overHeat = true and SecondTemp
int HeatMaxValue = 85;
int HeatMax = 140;
int HeatMin = 70;

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
bool HeatMaxSet = false;
bool FanOn = false;
bool FanHumOn = false;
bool TempHigh = false;
bool showSecondTemp = false;
bool showLed = false;
bool showLedSet = false;
bool LedOn = false;
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

IJsonDocument _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + std::string(mqtt_client_id);
  _json_this_device_doc["name"] = "drybox";
  _json_this_device_doc["sw_version"] = "2025-01-06";
  _json_this_device_doc["model"] = "Arduino Nano ESP32-S3";
  _json_this_device_doc["manufacturer"] = "Werner Krenn";
}
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "drybox".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, "drybox", _json_this_device_doc);

HaEntityNumber _ha_entity_mode(ha_bridge, "Mode", "drybox_mode");	//0=off, 1=on, 2=autooff, 3=autohum

HaEntitySwitch _ha_entity_switch_heater(ha_bridge, "Switch Heater", "drybox_switch_heater");
HaEntitySwitch _ha_entity_switch_fan(ha_bridge, "Switch Fan", "drybox_switch_fan");
HaEntitySwitch _ha_entity_switch_led(ha_bridge, "Switch Led", "drybox_switch_led");

HaEntityTemperature _ha_entity_temperature_target(ha_bridge, "Temperature Target", "drybox_temperature_target");
HaEntityHumidity _ha_entity_humidity_target(ha_bridge, "Humidity Target", "drybox_humidity_target");
HaEntityTemperature _ha_entity_temperature(ha_bridge, "Temperature", "drybox_temperature");
HaEntityHumidity _ha_entity_humidity(ha_bridge, "Humidity", "drybox_humidity");

HaEntityTemperature _ha_entity_temperature_2(ha_bridge, "Temperature 2", "drybox_temperature_2");
HaEntityHumidity _ha_entity_humidity_2(ha_bridge, "Humidity 2", "drybox_humidity_2");

bool _was_connected = false;
unsigned long _last_publish_ms = 0;


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
   LedOn = true;
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
    TargetTemp = int(TargetTemp *9/5) + 32);
    MaxSet = int(MaxSet *9/5) + 32);
    int Max = int(Max *9/5) + 32);
    int HeatMax = int(HeatMax *9/5) + 32);
    int HeatMin = int(HeatMin *9/5) + 32);

    int HeatMaxValue= Max;
    MinSet = int(MinSet *9/5) + 32);
    int FanCor = int(FanCor *9/5) + 32);
    int FanCor1 = int(FanCor1 *9/5) + 32);
    int FanCor2 = int(FanCor2 *9/5) + 32);
    //int FanCor3 = int(FanCor3 *9/5) + 32);
    //int FanCor4 = int(FanCor4 *9/5) + 32);
    int FanCor01 = int(FanCor01 *9/5) + 32);
    int FanCor02 = int(FanCor02 *9/5) + 32);
    int FanCor03 = int(FanCor03 *9/5) + 32);
    int FanCor04 = int(FanCor04  *9/5) + 32);

    char text[] = "°F"; 
   if (withDuct == true)
    {
     tempDiff = 0.18;
    } else {
     tempDiff = 0.36;
    } 
  #endif

  AutoOffTimeValue = AutoOffTime;
  FanValue = FanDelay;

  setupJsonForThisDevice(); // Populate json dict.

  // Setup WiFI
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("have wifi");
  Serial.print("IP number: ");
  Serial.println(WiFi.localIP());
  

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
    LedOn = false;
    } 
    #endif
    #ifndef Fahrenheit
    if ((showLed == true) and  (Temperature2 >= 60)) {
    showLedSet = true;
    digitalWrite(Led, LOW);
    LedOn = false;
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
       if (AutoHumValue < 25)
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

   mqtt();
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
      LedOn = true;
      delay(200);
     } 
     if((digitalRead(Button2) == LOW) and (showLed)) {                 // Action button 2: LED off
      showLed = false;
      showLedSet = false;
      LedOn = false;
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

    mqtt();
  }

}