# -Un-original-Prusa-Heated-Drybox Code for Ardunio (Nano) 
https://www.printables.com/de/model/883817-unoriginal-prusa-heated-drybox

Parts sold from Blurolls: processor ATmega328P (OldBootloader)

![DryBox](https://github.com/user-attachments/assets/9d7f4efc-d48a-4369-b78b-b287c5fc9cd7)

![Heater_off](https://github.com/user-attachments/assets/19ebee2a-b499-4e50-a4b3-5955bad877ee)
![Heater_on](https://github.com/user-attachments/assets/a007e035-a8de-42e3-872a-942b5bdcf179)

![Heater_offF](https://github.com/user-attachments/assets/02681fed-7551-4cb5-92fd-f77afdf04b4e)
![TargetTempF](https://github.com/user-attachments/assets/f6c2219c-9765-45e8-b5a4-053db0204314)


![With duct](https://github.com/user-attachments/assets/7fc5b59c-0aac-4b53-aea3-175a320b5d3d)
![Drybox_with_duct](https://github.com/user-attachments/assets/e4900aaa-83cf-4a48-a0a8-7c7a8acd7120)

## Heating test series:
```
Sensor Ecowitt WN34AD Temperature range -55..+125°C, Measuring cycle 77 seconds
```
![Ecowitt WN34AD_0](https://github.com/user-attachments/assets/2acd563e-9875-46dd-b9a1-2b5507f4d982)
```
Sensor DHT21  Temperature range -40..+80°C, here Measuring cycle 5 seonds
```
![DHT21](https://github.com/user-attachments/assets/9c1d9851-4e93-447c-ad65-1c769708b2a5)
```
Empty Box! With Duct from "morap"
WN34AD in the measuring hole of the duct
Second sensor DHT21 at the bottom of the box under the duct
```
![Messung](https://github.com/user-attachments/assets/d011207d-e09e-4f0a-a81a-9109b42e64a6)

```
DHT21 Duct: Previous measurement with DHT21 on the duct and temperature limitation at 123°C
```
```
All values are in °C - Time in minutes
Time 	WN34AD	Display	DHT21	DHT21 Duct
5	97.0	34.7	51.2	85
6	100.6	36.3	53.9	90
8	103.4	37.7	56.0	95
9	105.6	39.2	58.0
10	107.4	40.5	59.9
11	108.4	41.7	61.4	100
13	110.2	42.8	62.9	105
14	111.3	43.8	64.2	110
15	112.3	44.8	65.4
17	113.1	45.7	66.2	113	
18	113.8	46.6	67.4	115
19	114.5	47.3	68.2
21	115.1	48.0	69.0
22	115.6	48.7	69.5
23	116.1	49.3	70.2	119
24	116.6	49.9	70.8	
26	116.9	50.4	71.2	121
27	117.3	50.9	71.7	
28	117.6	51.4	72.0	122
29	118.0	51.8	72.4
31	118.2	52.2	72.7	123
32	118.5	52.5	73.0
33	118.8	52.8	73.4
35	119.0	53.1	73.7
36	119.3	53.4	73.9
38	119.6	53.9	74.4
40	120.0	54.2	74.7
41	120.1	54.4	74.9
42	120.2	54.6	75.0
43	120.3	54.8	75.2
45	120.5	55.0	75.4
46	120.7	55.1	75.5
47	120.8	55.3	75.6
49	121.0	55.4	75.8
```
![Diagramm](https://github.com/user-attachments/assets/a7e9c619-54d4-4593-ade5-b7aaff6dce8e)
```
3DL shows temperature curve at the rear left.
3D shows temperature curve at the front right
Depends greatly on the temperature at the back in the middle on the floor (directly under the duct last 75.8°C)!

```
### End of testing:
![End](https://github.com/user-attachments/assets/b67fae1c-2446-4816-93d6-413f0dfbe1f7)
![Ecowitt WN3434AD_1](https://github.com/user-attachments/assets/8cfddba4-f08c-49e9-a29e-498368c2b26d)



## Code files for the heater 

### Main file = Serial.ino
![Arduino IDE](https://github.com/user-attachments/assets/ad56c513-1821-44f5-b948-361721efd630)


Support a second temperature/humidity sensor (DHT21 or DHT22)
You need then additional the Arduino library "DHT-sensor-library"

![SecondSensor1](https://github.com/user-attachments/assets/ab4d7196-bf3d-40a5-a764-5d4ee09c2fb8)
![SecondSensor2](https://github.com/user-attachments/assets/99cd2411-968e-4d7e-8500-d323ce40b612)


Control the fan via a second mosfet
Can Control a led strip via a third mosfet
Heater AutoOff function.
Heater AutoHum function.
Overheating control (with second temperatur/humidity sensor) - not active if no second temp/hum semsor or disabled

Fan control: Vin+ to 24V+, Vin- to 24V-(Gnd), Out+ to Fan red, Out- to Fan black, Gnd to Buck-Convert Out- or Arduino GND
//Arduino pin where the control line for the Mosfet is connected
```
#define Fan 5
```
Led strip control: Vin+ to 24V+, Vin- to 24V-(Gnd), Out+ to Led red, Out- to Led black, Gnd to Buck-Convert Out- or Arduino GND
24V Led strip:
```
#define controlLed
#define Led 4
```
![Led_strip_wire](https://github.com/user-attachments/assets/1af6b4a9-6b81-408e-a03d-740f8c64cf99)
![Led_strip1](https://github.com/user-attachments/assets/423c547b-8754-46cb-bdc2-d03944e5a795)
![Led_strip_on](https://github.com/user-attachments/assets/8a4ddedd-d2d5-4632-a0f2-fccb9b7ee07f)


In the program, the second sensor is switched on with
```
#define SecondTemp
or deactivated with //#define SecondTemp
```
The sensor used is specified here
```
#define DHT_TYPE DHT21
//#define DHT_TYPE DHT22
```
The DHT21 is activated here
The data pin of this sensor is set to 9:
```
#define DHT_PIN 9
```
The other pins of this second sensor are to be connected to 5V and GND.

If the second sensor is activated but there is no second sensor,
0.0% and 0.0°C are displayed in the data line.

## Protection for the Wires
![High temperature wire protection](https://github.com/user-attachments/assets/1860929f-a979-4548-8b35-190cc300163e)



If the heating is switched off, you can use the
#### "-" button (= bottom button) to switch on or off to the normal display (no display of the data from the second sensor).
-> Function not available if no second temp/hum sensor present

If the heating is switched off, you can use the
#### "+" button (= middle button) to toggle the LED strip off/on - default is on

In addition, you can use the program to display the data from the temperature/humidity sensors using
```
#define debug (default deactivated with "//#define debug")
```
via the IDE's serial monitor.
But this data are not updated if the AutoOff function is activ!

It is possible to correct the temperature and/or humidity values with a correction value.
```
float TemperatureCor = 0.0;
float HumidityCor = 0.0;
float Temperature2Cor = 0.0;
float Humidity2Cor = 0.0;
//The maximum adjustable temperature is limited to 70°C.
int MaxSet = 70;  
//The overheating temperature is set to 83°C - now measured under the duct!
int Max = 83;   
```
This is because the Arduino should only be operated up to a maximum temperature of 80°C.
If you want a higher temperature, you have to place the electronic components outside the housing! 

When the heating is switched on, you can activate the heating AutoOff mode by pressing the

 ### First and hold the "+" (middle button) and then "on/off" (above button) button.
You may have to try a few times until it works. The display shows then "AutoOff"

![AutoOff](https://github.com/user-attachments/assets/677255e3-7602-4d73-b343-3711d502cdc9)


The default switch-off value is 360 minutes
```
int AutoOffTime = 360;
```
The maximum time for switching off is 2 days (2880 minutes)
```
int MaxAutoOffTime = 2880; // max 2 days, step 10 minutes
```
The minimum value is 10 minutes.

When the heating is switched on, you can activate the heating AutoHum mode by pressing the
 ### First and hold the "-" (below button) and then "on/off" (above button) button.
You may have to try a few times until it works.  The display shows then "AutoHum"

![AutoHum](https://github.com/user-attachments/assets/97c53ffc-8a5c-47c9-a0d7-6cbc97609373)


If the set Humidity value (10-65%), default 35% is reached the heater is switched off
the Fan delay starts and after the set time the fan is also switched off (only with extra Mosfet for fan!)
-> stand by. 
If the Humidity value increase the heater and fan is switched on again
and so on. 

The default Auto Hum value is 25 % - changed from 35% to 25%
```
int TargetHum = 25;
```
The maximum percent for standby = 65 %		// with so high value you can simple test the AutoHum function!
The minimum value is 10%			// This humidity level will probably not be reached with heating!

![2024-08-28 14 24 20](https://github.com/user-attachments/assets/c4e86f0e-9a17-4532-97dc-66cc20944308)
![2024-08-28 14 34 04](https://github.com/user-attachments/assets/572a69a7-5793-462f-8bdd-c80798b78fcc)
```
 On the picture you see a AutoHum value of 45% and the Heater is on (ON)
 The humidity value is 44.4% (AutoHum value reached) the Heater is off (on)
 The fan delay starts and the fan is also switched off in 230 seconds (need fan control).
 If the humidity rised to 46% the heater and fan is ON again. 
```
The settings of the AutoOff Minutes and AutoHum times is now automatical done after 30 seconds!
A dot will appear on the far right side if the change of the values of AutoOff or AutoHum is still active.
Otherwise, change the target temperature.

![Settings](https://github.com/user-attachments/assets/62b36232-0031-49fe-a8d5-0dda6c6f71aa)

### Fan Control:
Function: when the heater is switched on, the fan always runs, even if the target temperature has been reached (heating element is switched off).
If the heating element is switched on in normal mode, this is now displayed with "ON".
If switched off, it is displayed with "on".
In AutoOff mode, a switched on heating element is displayed with "A" and a white background, 
if switched off, it is displayed with "A" and a black background.
When overtemperature is reached, "Hi" is displayed instead of "ON" and the heater is switched off until the temperature drops again.
In AutoOff mode instead of "A" then "H"

 ![Vertical Display hiTemp](https://github.com/user-attachments/assets/e630dfd2-9fed-4148-8150-7f9b5135c053)

When the heater is switched off (directly or via AutoOff), the fan continues to run for an adjustable time:
```
int FanDelay = 300;
```
![Vertical Display](https://github.com/user-attachments/assets/32e77ae5-27ff-4cd1-b51f-0e76d06c6672)

This run-on time is then counted down in the display after switching off.
The fan shutdown time now depends on the selected target temperature (60 - 900 seconds)

If you have installed a second temperature sensor, you can with 
```
bool overHeat = true;
```
monitor the temperature directly on the heating element and when the max temperature (°C)
```
int Max = 83;		// = 181 °F
```
is reached, the heating is also switched off (fan continues to run if second Mosfet is used)

Other settings:
```
#define showFraction
```
Now also in the State Line the Temperatur or Humidity are shown with fraction.
If you like the former showing (only Integer) uncomment //#define showFraction

![AutoOff_HeaterOn_secondSensor_showFraction](https://github.com/user-attachments/assets/a1cd961f-3ddd-4f4c-bae2-ebb3d10f07f5)
![AutoOff_HeaterOn_secondSensor_not_showFraction](https://github.com/user-attachments/assets/3bcbc863-9e21-43b3-ad70-5486f358f208)


If Duct is installed, the system is slower, so the change temperature is set to 0.2°C here, otherwise 0.5°C
```
bool withDuct = true;                     // Duct installed -> Tempdiff = 0.2°C otherwise 0.5°C
```

### But be careful: 
    all electronic components used (temp sensor, Arduino, etc.) are only designed for a maximum ambient temperature of 80°C (176°F).
    And you should also note: Maximum operating temperature of the fan is 70°C
    If using inside the (Un)original Prusa Heated Drybox a LED strip you should know:
    the maximum operating temperature of the LED strip is normally 60°C! 

#### Warning!
     If the fan control is not used, the fans must be permanently connected to the 24 V voltage.
     The display behaves as if the additional fan control were present, showing a countdown in seconds for the fan to shut down!

#### My recommendation for the filament to be used for "Bracket Y" and "condotto 5" is PC blend 
#### or an even higher temperature resistant filament.
#### In my experience, even ASA is not suitable!

### Also important!
    When the heating is on, air exchange must be able to take place, 
    e.g. by lifting the front door or using the unused PTFE outlets, 
    otherwise humidity reduction cannot take place.
![2024-08-29 10 37 31](https://github.com/user-attachments/assets/7d6a6abf-3142-47a8-ad62-016dbb75a41a)
    ![2024-08-29 10 37 23](https://github.com/user-attachments/assets/fd8e3088-1ef1-4410-a763-4399379b0c08)

