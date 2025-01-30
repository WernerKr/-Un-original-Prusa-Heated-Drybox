// Uncomment the next line if you want the temperature to be displayed in Fahrenheit, it is displayed in Celcius by default
// #define Fahrenheit

// to see the Temp/Hum Data and DS18B20 addresses in the Serial Monitor uncomment next line
#define debug

#define showFraction		// all values with decimal values otherwise only integer values
// If an LED strip needs to be controlled otherwise uncomment next line
#define controlLed
#define ControllLedAutooff	// Led on and Temperature >=60°C/140°F (= max. operating temperature LED strip) the led is switched off

// if have not attached a second temp/hum Sensor (dht22 or dht21) uncomment next line
#define SecondTemp		// should be placed on the floor under the heater or duct - not tested without this!
			// Showing of SecondTemp is now always on! Is needed for DS18B20 sensors
#define BlankScreen		// activate ScreenSaver with "down" Button - 
#define UsePixel		// ScreenSaver is "running Pixel" otherwise blank Screen
#define BlankScreenAutoOff            // if heating is off sreensaver active after 10 minutes - need also #UsePixel

#define WATCHDOG

//#define HomeAssistant		// send data to Homee Assistant

#ifdef HomeAssistant

 char HAname[12] = "Drybox2";
 char HAnamelower[12] = "drybox2";

 char HAmodetx[] = "Mode";
 char HAmode[] = "Mode";
 char HAsheater[] = "Switch Heater";
 char HAsfan[] = "Switch Fan";
 char HAsled[] = "Switch Led";

 char HAtemptarg[] = "Temperature Target";		//Sensor Name in Home Assistant
 char HAhumtarg[] = "Humidity Target";
 char HAtemp[] = "Temperature";
 char HAtemp2[] = "Temperature 2";
 char HAhum[] = "Humidity";
 char HAhum2[] = "Humidity 2";

 char HAmodetxttxt[] = "drybox_modetxt";
 char HAmodetxt[] = "drybox_mode";
 char HAsheatertxt[] = "drybox_switch_heater";
 char HAsfantxt[] = "drybox_switch_fan";
 char HAsledtxt[] = "drybox_switch_led";

 char HAtemptargtxt[] = "drybox_temperature_target";	//Entity-ID ( sensor.drybox_temperature_target )
 char HAhumtargtxt[] = "drybox_humidity_target";
 char HAtemptxt[] = "drybox_temperature";
 char HAtemp2txt[] = "drybox_temperature_2";
 char HAhumtxt[] = "drybox_humidity";
 char HAhum2txt[] = "drybox_humidity_2";

#define MQTT_ID "DryBox2"
#define WIFI_SSID "FRITZ!Box"		// your network SSID (name)
#define WIFI_PASS "????????????????????"	// your network password (use for WPA, or use as key for WEP)
#define MQTT_HOST "192.???.???.???"
#define MQTT_USER "Name"
#define MQTT_PASS "Password"

#endif

//#define DS18B20xx		// also support and show DS18B20 temperature sensors

#ifdef DS18B20xx
#define DS18B20HA         	//DS18B20 data also send to Home Assinstant

uint8_t sensor1[8] = { 0x28, 0x2C, 0xA8, 0x83, 0x00, 0x00, 0x00, 0x89 };
uint8_t sensor2[8] = { 0x28, 0x2B, 0x73, 0x85, 0x00, 0x00, 0x00, 0x2B };
uint8_t sensor3[8] = { 0x28, 0x57, 0x8A, 0x84, 0x00, 0x00, 0x00, 0x14 };

//#define DallasAdrSet		// are the DS18B20 temp sensors adresses above correct? Can be found with debug and seriell Monitor

#define DallasSensor1		// first DS18B20 temp sensors available
#define DallasSensor2		// second DS18B20 temp sensors available
#define DallasSensor3		// third DS18B20 temp sensors available

#ifdef HomeAssistant
 char HAsensor1[] = "Temperature e1";		//Sensor Name in Home Assistant
 char HAsensor2[] = "Temperature e2";
 char HAsensor3[] = "Temperature e3";

 char HAsensor1txt[] = "drybox_temperature_e1";		//Entity-ID ( sensor.drybox_temperature_e1 )
 char HAsensor2txt[] = "drybox_temperature_e2";
 char HAsensor3txt[] = "drybox_temperature_e3";
#endif

#endif

float TemperatureCor = 0.0;               // correction Temperatur
float HumidityCor = 0.0;                  // correction Humidity
float Temperature2Cor = 0.0;              // correction Temperatur2 if #define SecondTemp 
float Humidity2Cor = 0.0;                 // correction Humidity2 if #define SecondTemp

// all this Temperature values are need in °C
// If you select Fahrenheit, these values will later be converted for use in Fahrenheit
int Max = 85;                             // overHeating Temp under the duct, need #overHeat = true and SecondTemp
int HeatMaxValue = 85;
int HeatMax = 140;
int HeatMin = 70;

bool overHeat = true;                     // control overheating - only used if second temp/hum is available
bool withDuct = true;                     // Duct installed -> Tempdiff = 0.1°C otherwise 0.3°C
int TargetTemp = 50;
int Refresh = 2000;                       // Update values in msec if heater is on (former 5000)
int MaxSet = 70;                          // max allowed setting TargetTemp °C
int MinSet = 20;                          // min allowed setting TargetTemp °C
int AutoOffTime = 360;                    // here 6 hours
int MaxAutoOffTime = 2880;                // max 2 days
int FanDelay = 300;                       // 300 seconds=5 minutes (max 999 seconds supported)
int FanCor = 50;                          // >= 50°C TargetTemp add to Fandelay 60 or 90 sec
int FanCor1 = 55;                         // all FanCor are also in °C - are later converted to Fahrenheit if set
int FanCor2 = 60;
//int FanCor3 = 65;
//int FanCor4 = 70;
int FanCor01 = 40;
int FanCor02 = 35;
int FanCor03 = 30;
int FanCor04 = 25;

int TargetHum = 25;

int MaxHumSet = 65;                        // max allowed setting TargetHum
int MinHumSet = 10;
float humDiff = 0.2;                       // diff between hum values on/off

int screenSaverTimeOut = 600;            // AutoOn Screensaver after 10 minutes / 600 sec
