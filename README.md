# -Un-original-Prusa-Heated-Drybox Code for Ardunio (Nano) 
sold from BlueRolls: processor ATmege328P (OldBootloader)
Code files for the heater 

Main file = serial.ino


Support a second temperature/humidity sensor (DHT21 or DHT22)
You need then additional the Arduino library "DHT-sensor-library"

Control the fan via a second mosfet
Can Control a led strip via a third mosfet
Heater AutoOff function.
Heater AutoHum function.
Overheating control (with second temperatur/humidity sensor) - not active if no second temp/hum semsor or disabled

Fan control: Vin+ to 24V+, Vin- to 24V-(Gnd), Out+ to Fan red, Out- to Fan black, Gnd to Buck-Convert Out- or Arduino GND
//Arduino pin where the control line for the Mosfet is connected
#define Fan 5

Led strip control: Vin+ to 24V+, Vin- to 24V-(Gnd), Out+ to Led red, Out- to Led black, Gnd to Buck-Convert Out- or Arduino GND
24V Led strip:
#define controlLed
#define Led 4

In the program, the second sensor is switched on with
#define SecondTemp
or deactivated with //#define SecondTemp

The sensor used is specified here
#define DHT_TYPE DHT21
//#define DHT_TYPE DHT22
The DHT21 is activated here
The data pin of this sensor is set to 9:
#define DHT_PIN 9
The other pins of this second sensor are to be connected to 5V and GND.

If the second sensor is activated but there is no second sensor,
0.0% and 0.0°C are displayed in the data line.

If the heating is switched off, you can use the
"-" button (= bottom button) to switch on or off to the normal display (no display of the data from the second sensor).
-> Function not available if no second temp/hum sensor present

If the heating is switched off, you can use the
"+" button (= middle button) to toggle the LED strip off/on - default is on

In addition, you can use the program to display the data from the temperature/humidity sensors using
#define debug (default deactivated with "//#define debug")
via the IDE's serial monitor.
But this data are not updated if the AutoOff function is activ!

It is possible to correct the temperature and/or humidity values with a correction value.
float TemperatureCor = 0.0;
float HumidityCor = 0.0;
float Temperature2Cor = 0.0;
float Humidity2Cor = 0.0;
//The maximum adjustable temperature is limited to 70°C.
int MaxSet = 70;  
//The overheating temperature is set to 93°C
int Max = 93;   

This is because the Arduino should only be operated up to a maximum temperature of 80°C.
If you want a higher temperature, you have to place the electronic components outside the housing! 

When the heating is switched on, you can activate the heating AutoOff mode by pressing the
CHANGED!
First and hold the "+" (middle button) and then "on/off" (above button) button.
You may have to try a few times until it works. The display shows then "AutoOff"

The default switch-off value is 360 minutes
int AutoOffTime = 360;

The maximum time for switching off is 2 days (2880 minutes)
int MaxAutoOffTime = 2880; // max 2 days, step 10 minutes
The minimum value is 10 minutes.

When the heating is switched on, you can activate the heating AutoHum mode by pressing the
First and hold the "-" (middle below button) and then "on/off" (above button) button.
You may have to try a few times until it works.  The display shows then "AutoHum"

If the set Humidity value (10-65%), default 35% is reached the heater is switched off
the Fan delay starts and after the set time the fan is also switched off (only with extra Mosfet for fan!)
-> stand by. 
If the Humidity value increase the heater and fan is switched on again
and so on. 

The default Auto Hum value is 35 %
int TargetHum = 35;
The maximum percent for standby = 65 %		// with so high value you can simple test the AutoHum function!
The minimum value is 10%			// This humidity level will probably not be reached with heating! 

The settings of the AutoOff Minutes and AutoHum times is now automatical done after 30 seconds!
A dot will appear on the far right side if the change of the values of AutoOff or AutoHum is still active.
Otherwise, change the target temperature.

Fan Control:
Function: when the heater is switched on, the fan always runs, even if the target temperature has been reached (heating element is switched off).
If the heating element is switched on in normal mode, this is now displayed with "ON".
If switched off, it is displayed with "on".
In AutoOff mode, a switched on heating element is displayed with "A" and a white background, 
if switched off, it is displayed with "A" and a black background.
When overtemperature is reached, "Hi" is displayed instead of "ON" and the heater is switched off until the temperature drops again.
In AutoOff mode instead of "A" then "H"
 
When the heater is switched off (directly or via AutoOff), the fan continues to run for an adjustable time:

int FanDelay = 300;
This run-on time is then counted down in the display after switching off.
The fan shutdown time now depends on the selected target temperature (60 - 900 seconds)

If you have installed a second temperature sensor, you can with 
bool overHeat = true;
monitor the temperature directly on the heating element and when the max temperature (°C)
int Max = 93;		// = 199 °F
is reached, the heating is also switched off (fan continues to run if second Mosfet is used)

Other settings:
#define showFraction
Now also in the State Line the Temperatur or Humidity are shown with fraction.
If you like the former showing (only Integer) uncomment #define showFraction

If Duct is installed, the system is slower, so the change temperature is set to 0.2°C here, otherwise 0.5°C
bool withDuct = true;                     // Duct installed -> Tempdiff = 0.2°C otherwise 0.5°C


But be careful: 
all electronic components used (temp sensor, Arduino, etc.) are only designed for a maximum ambient temperature of 80°C (176°F).
And you should also note: Maximum operating temperature of the fan is 70°C
If using inside the (Un)original Prusa Heated Drybox a LED strip you should know:
the maximum operating temperature of the LED strip is normally 60°C! 

