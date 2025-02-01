/* 
Code for the (Un)original Prusa Drybox Heater
Written by Bram Elema, Creative Commons BY-NC-SA

Added support for a second temp/hum Sensor DHT21 or DHT22
and AutoOff Function 
and AutoHum Function
and new Fan control, and Led Strip control
If heater is set on, the data now updated each 2 sec otherwise 10 sec 
The TargetTemp is now 50°C, the AutoHum ist now 25%
Update Display if Heating on is now 2 sec, tempDiff is now 0.1°C/0.18°F
Caution! Led Pin is now 3 (was former 4) 
         because DS18B20 works for me only on PIN 4 = GPIO7 !!
Werner Krenn - last modified 
*/
char swversion[12] = "2025-02-01";
/*
REQUIRES the following Arduino libraries:
 - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_SSD1306 Library: https://github.com/adafruit/Adafruit_SSD1306
 - DFRobot_DHT20 Sensor Library: https://github.com/DFRobot/DFRobot_DHT20
 - DHT_sensor_library Library: https://github.com/adafruit/DHT-sensor-library
 - OneWire
 Arduino Nano ESP32:
 - Arduino ESP32
 - ESP32 core v3.x.x
 HA MQTT:
 - ArduinoJson https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 - MQTTRemote
 - MQTT 
 - HomeAssistantEntities https://github.com/Johboh/HomeAssistantEntities
 DS18B20 Temperature Sensor:
 - DallasTemperature
*/

#include <Arduino.h>
// This is for the Arduino IDE, where we always build with ArduinoJson. arduino-cli will not build/include libraries
// that are not included anywhere. So we must include ArduinoJson.h so its available for IJson.h later.
// For Platform IO, this is not the case and these examples are built both with ArduinoJson and nlohmann-json.


#include "arduino_settings.h"

#include <esp_task_wdt.h>
#define WDT_TIMEOUT 20 
esp_err_t ESP32_ERROR;

#ifndef PLATFORMIO
#include <ArduinoJson.h>
#endif

#ifdef HomeAssistant
 #include <HaBridge.h>
 // This is here for CI only. You can remove this include and include ArduinoJson.h or nlohmann/json.hpp directly.
 #include <IJson.h>
 #include <MQTTRemote.h>

 #include <entities/HaEntitySwitch.h>
 #include <entities/HaEntityNumber.h>
 #include <entities/HaEntityText.h>
 #include <entities/HaEntityString.h>
 #include <entities/HaEntityTemperature.h>
 #include <entities/HaEntityHumidity.h>
#endif

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


//DallasTemperature
#ifdef DS18B20xx

#include <DallasTemperature.h>
#define ONE_WIRE_BUS 7			//D4 - GPIO7, 
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

DeviceAddress Thermometer;
int deviceCount = 0;
float sensor1val = 0;
float sensor2val = 0;
float sensor3val = 0; 
float tempC;

#endif

#define Button1 6                       // on/off
#define Button2 7                       // + increase
#define Button3 8                       // - decrease  
#define Heater 10
#define Fan 5

//#define Led 3

#ifdef DS18B20xx
 #define Led 3
#endif
#ifndef DS18B20xx
 #define Led 4
#endif

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

bool FanRun = false;

bool HumOff = false;                       // AutoHum trigger 

unsigned long previousMillis = 0;
unsigned long AutoOffpreviousMillis = 0;
unsigned long AutoOffSetpreviousMillis = 0;
unsigned long FanOnpreviousMillis = 0;
unsigned long FanHumOnpreviousMillis = 0;
unsigned long debugMillis = 0;

#ifdef BlankScreen
#ifdef UsePixel
#ifdef BlankScreenAutoOff
unsigned long ScreenSaverpreviousMillis = 0;
#endif
#endif
#endif

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
bool LedOn = false;
char text[] = "°C";
#ifdef debug
char textC[] = "°C";
char textF[] = "°F";
#endif 

float Temperature;
float Humidity;
float Temperature2;
float Humidity2;
float tempDiff;
int TempSensor = 3;
int TempSensorMax = 0;
//int SensorCnt = 0;
float TempVal;
bool printAdr = false;
int device = 0;

int bild_h = 0;
int bild_w = 0; 
int bildpos = 0; 
bool screensaver = false;
bool screensaverOn = false;

char TempIntegerDisplay[4];
char TempFractionDisplay[4];
char HumIntegerDisplay[4];
char HumFractionDisplay[4];
char TargetTempDisplay[4];
char TargetHumDisplay[2];
char MinuteDisplay[4];

#ifdef HomeAssistant

 const char wifi_ssid[] = WIFI_SSID; 
 const char wifi_password[] = WIFI_PASS; 
 const char mqtt_client_id[] = MQTT_ID;
 const char mqtt_host[] = MQTT_HOST;
 const char mqtt_username[] = MQTT_USER;
 const char mqtt_password[] = MQTT_PASS;

IJsonDocument _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + std::string(mqtt_client_id);
  _json_this_device_doc["name"] = HAname;
  _json_this_device_doc["sw_version"] = swversion;
  _json_this_device_doc["model"] = "Arduino Nano ESP32-S3";
  _json_this_device_doc["manufacturer"] = "Werner Krenn";
}
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "drybox".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, HAnamelower, _json_this_device_doc);

HaEntityString _ha_entity_modetext(ha_bridge, HAmodetx, HAmodetxttxt);
HaEntityNumber _ha_entity_mode(ha_bridge, HAmode, HAmodetxt);	//1=off, 2=on, 3=autooff, 4=autohum

HaEntitySwitch _ha_entity_switch_heater(ha_bridge, HAsheater, HAsheatertxt);
HaEntitySwitch _ha_entity_switch_fan(ha_bridge, HAsfan, HAsfantxt);
#ifdef controlLed
 HaEntitySwitch _ha_entity_switch_led(ha_bridge, HAsled, HAsledtxt);
#endif

#ifndef Fahrenheit

HaEntityTemperature _ha_entity_temperature_target(ha_bridge, HAtemptarg, HAtemptargtxt);
HaEntityTemperature _ha_entity_temperature(ha_bridge, HAtemp, HAtemptxt);
HaEntityTemperature _ha_entity_temperature_2(ha_bridge, HAtemp2, HAtemp2txt);

#ifdef DS18B20HA
 #ifdef DallasSensor1
  HaEntityTemperature _ha_entity_temperature_e1(ha_bridge, HAsensor1, HAsensor1txt);
 #endif
 #ifdef DallasSensor2
  HaEntityTemperature _ha_entity_temperature_e2(ha_bridge, HAsensor2, HAsensor2txt);
 #endif
 #ifdef DallasSensor3
  HaEntityTemperature _ha_entity_temperature_e3(ha_bridge, HAsensor3, HAsensor3txt);
 #endif
#endif

#endif

#ifdef Fahrenheit
HaEntityTemperature _ha_entity_temperature_target(ha_bridge, HAtemptarg, HAtemptargtxt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});
HaEntityTemperature _ha_entity_temperature(ha_bridge, HAtemp, HAtemptxt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});
HaEntityTemperature _ha_entity_temperature_2(ha_bridge, HAtemp2, HAtemp2txt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});

#ifdef DS18B20HA
 #ifdef DallasSensor1
  HaEntityTemperature _ha_entity_temperature_e1(ha_bridge, HAsensor1, HAsensor1txt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});
 #endif
 #ifdef DallasSensor2
  HaEntityTemperature _ha_entity_temperature_e2(ha_bridge, HAsensor2, HAsensor2txt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});
 #endif
 #ifdef DallasSensor3
  HaEntityTemperature _ha_entity_temperature_e3(ha_bridge, HAsensor3, HAsensor3txt,
                                           HaEntityTemperature::Configuration{.unit = HaEntityTemperature::Unit::F,
                                                                              .force_update = false});
 #endif
#endif

#endif

HaEntityHumidity _ha_entity_humidity_target(ha_bridge, HAhumtarg, HAhumtargtxt);
HaEntityHumidity _ha_entity_humidity(ha_bridge, HAhum, HAhumtxt);
HaEntityHumidity _ha_entity_humidity_2(ha_bridge, HAhum2, HAhum2txt);

#endif

bool _was_connected = false;
unsigned long _last_publish_ms = 0;
int iCount = 0; 


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
  pinMode(LED_BLUE, OUTPUT);
  //pinMode(LED_BUILTIN, OUTPUT);

  #ifdef SecondTemp
    dht.begin(); // initialize the sensor
    showSecondTemp = true;
  #endif

  #ifdef controlLed
   showLed = true;
   digitalWrite(Led, HIGH);
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

  #ifdef HomeAssistant
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

  #endif 

  #ifdef DS18B20xx

   #ifdef DallasSensor1
    TempSensorMax += 1; 
   #endif
   #ifdef DallasSensor2
    TempSensorMax += 1; 
   #endif
   #ifdef DallasSensor3
    TempSensorMax += 1; 
   #endif
   showSecondTemp = true;

   sensors.begin();
   //sensors.setResolution(TEMP_12_BIT); // Genauigkeit auf 12-Bit setzen
   /*
     9 Bit: 0,5 °C
    10 Bit: 0,25 °C
    11 Bit: 0,125 °C
    12 Bit: 0,0625 °C
   */
   delay(1000);
   sensors.requestTemperatures(); 

   Serial.println("Locating devices...");
   Serial.print("Found ");
   deviceCount = sensors.getDeviceCount();
   Serial.print(deviceCount, DEC);
   Serial.println(" devices.");
   Serial.println("");

  #endif

#ifdef WATCHDOG
  Serial.println("ESP started ...");            //Say hello to the world
  esp_task_wdt_deinit();
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = WDT_TIMEOUT * 1000,                 // Convertin ms
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,  // Bitmask of all cores, https://github.com/espressif/esp-idf/blob/v5.2.2/examples/system/task_watchdog/main/task_watchdog_example_main.c
    .trigger_panic = true                             // ... ?Enable panic to restart ESP32
  };
  // WDT Init
  ESP32_ERROR = esp_task_wdt_init(&wdt_config);
   Serial.println("Last Reset : " + String(esp_err_to_name(ESP32_ERROR)));
   esp_task_wdt_add(NULL);  //add current thread to WDT watch

   //Serial.println(" Resetting WDT...");
   //esp_task_wdt_reset();
   // Serial.println("Disable watchdog");   //Write information to serial monitor
   // esp_task_wdt_deinit();                // Disable watchdog
#endif

  digitalWrite(LED_BLUE, HIGH);
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
            FanRun = true;
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
        FanRun = false;
     }
     if ( (Hot == false) and (FanValue <=0)){
        digitalWrite(Fan, LOW);
        FanRun = false;
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
       FanRun = true;
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
      if (screensaverOn == true) {
       printPixel();
      } else {       
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
      }
      display.display();    
      heater();
    }
    if (overHeat == true)
    {
      if ((TempHigh == false) and (Hot == true))
      {
        digitalWrite(Heater, HIGH);
        screensaverOn = false; 
      }
    }

  #ifdef HomeAssistant
   mqtt();
  #endif
  
  #ifdef WATCHDOG
   esp_task_wdt_reset();
   delay(1); 
  #endif 

  }

  // Heating system OFF:
  while(status == false){
    unsigned long currentMillis = millis();
    AutoOff = false;
    AutoOffSet = false;
    AutoOffTimeValue = AutoOffTime;
    digitalWrite(Heater, LOW);
    Hot = false;
    if (FanOn == true)
    {   
    FanValue = FanDelay - ((currentMillis - FanOnpreviousMillis)/1000);  
    if((currentMillis - FanOnpreviousMillis)/1000 >= FanDelay) {     // Switch off Fan delay
      FanOn = false;
      digitalWrite(Fan, LOW);
      FanRun = false;
      }
    }

    #ifdef BlankScreen 
    #ifdef UsePixel
    #ifdef BlankScreenAutoOff
    if ((screensaver == false) and (screensaverOn == false) and (FanValue <=0)) {
      ScreenSaverpreviousMillis = currentMillis;
      screensaver = true;
      }
     if (screensaverOn == false) { 
      if((currentMillis - ScreenSaverpreviousMillis)/1000 >= screenSaverTimeOut) {
       screensaverOn = true;
       ScreenSaverpreviousMillis = currentMillis;
      }  
     }
    #endif
    #endif 
    #endif

    if(digitalRead(Button1) == LOW) {                 // Action button 1: switch heating system from off to on
      status = !status;
      sensorUpdate();
      if (status == true) {
       digitalWrite(Fan, HIGH);
       FanRun = true;
       FanOn = true;
       screensaverOn = false;
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

    #ifndef BlankScreenAutoOff  
     if ((screensaver == true) and (FanValue <=0)) { screensaverOn = true; }
    #endif

    // if((digitalRead(Button3) == LOW) and (not showSecondTemp)) {                 // Action button 3: second Temp view on
    if(( digitalRead(Button3) == LOW )) {                 // Action button 3: ScreenSaver ?
      showSecondTemp = true;
      #ifdef BlankScreen
       screensaver = !screensaver;
       screensaverOn = screensaver;
       #ifdef UsePixel
       #ifdef BlankScreenAutoOff     
        if (screensaverOn == false) {ScreenSaverpreviousMillis = currentMillis;}
       #endif
       #endif 
      #endif 

      display.clearDisplay();
      if (screensaverOn == true){
      printPixel();
      } else {      
      drawStatus();
      drawTemperature1();
      drawHumidity1();
      drawTemperature2();
      drawHumidity2();
      }
      display.display();
      delay(200);
     } 
     #ifndef DS18B20xx
     if((digitalRead(Button3) == LOW) and (showSecondTemp)) {                 // Action button 3: second Temp view off
      showSecondTemp = false;
      display.clearDisplay();
      drawStatus();
      drawTemperature();
      drawHumidity();
      display.display(); 
      delay(200);
     } 
     #endif 
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
      if (screensaverOn == true){
      printPixel();
      } else {      
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
      }
      display.display();
    }

   #ifdef HomeAssistant
    mqtt();
   #endif

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
   
    #ifdef DS18B20xx

    if (printAdr == false){
     Serial.println();
     Serial.println();
     Serial.println("Printing addresses...");
     for (int i = 0;  i < deviceCount;  i++)
     {
      Serial.print("Sensor ");
      Serial.print(i+1);
      Serial.print(" : ");
      sensors.getAddress(Thermometer, i);
      printAddress(Thermometer);
      printAdr = true;
     }
     Serial.println();
     Serial.println();
    }

     for (int i = 0;  i < deviceCount;  i++)
     {
    device = i+1,
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print(textC);
    Serial.print("  |  ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    Serial.print(textF);
    Serial.println(" ");
    }
    Serial.println("");
    #endif

    }
    #endif
  #ifdef WATCHDOG
   esp_task_wdt_reset();
   delay(1); 
  #endif 
  }
}

#ifdef DS18B20xx
void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}
#endif