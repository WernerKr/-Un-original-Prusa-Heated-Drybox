// Uncomment the next line if you want the temperature to be displayed in Fahrenheit, it is displayed in Celcius by default
//#define Fahrenheit

// to see the Temp/Hum Data in the Serial Monitor uncomment next line
//#define debug

#define showFraction                    // all values with decimal values otherwise only integer values
// If an LED strip needs to be controlled otherwise uncomment next line
#define controlLed
#define ControllLedAutooff            // Led on and Temperature >=60°C/140°F (= max. operating temperature LED strip) the led is switched off

// if have not attached a second temp/hum Sensor (dht22 or dht21) uncomment next line
#define SecondTemp                    // should be placed on the floor under the heater or duct

float TemperatureCor = 0.0;               // correction Temperatur
float HumidityCor = 0.0;                  // correction Humidity
float Temperature2Cor = 0.0;              // correction Temperatur2 if #define SecondTemp 
float Humidity2Cor = 0.0;                 // correction Humidity2 if #define SecondTemp

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
int MaxHumSet = 65;                         // max allowed setting TargetHum
int MinHumSet = 10;
float humDiff = 0.2;                       // diff between hum values on/off

