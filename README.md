## -Un-original-Prusa-Heated-Drybox Code for Ardunio Nano ESP32-S3 Home Assistant

Required processor Arduino Nano ESP32-S3, is pin compatible with the Arduino Nano 
with the exception that the 5V pin only supplies a voltage when USB is connected. 
Therefore, no more 5V is taken from this pin.

For Johboh/HomeAssistantEntities you need >= 7.0.8, supports 
state_class:measurement;
which is needed for History Graphic 
- The former PIN D4 for Led is now D3. 
- The DS18B20 data works only with PIN D4.
- But to use it, the GPIO pin = 7 for D4 must be set.
- To connect the DS18B20 temperature sensors (3 are supported in the program code) a 4.7 KOhm resistor must be used between pin D4 and Vin (=5V). All data lines of the DS18B20 are connected to pin D4 at the same time.
- Since the Arduino Nano Esp32 has a higher internal temperature, the DHT20 heats up more and the temperature value is therefore distorted when the heating is off.
- Therefore, it makes sense to move/mount the DHT20 sensor outside the Z-bracket, perhaps above the Z-bracket near the fan
#### All user settings are now stored in the file arduino_settings.h.
- #define debug
- #define showFraction
- #define controlLed
- #define ControllLedAutooff
- #define SecondTemp
- #define HomeAssistant	
- security and lan settings for Home Assistant
- #define DS18B20xx
- #define DS18B20HA
- #define DallasAdrSet
- #define DallasSensor1
- #define DallasSensor2
- #define DallasSensor3
- all other settings (targetTemp, targetHum ...)

![Home Assistant](https://github.com/user-attachments/assets/1bd42809-0463-4e9a-9c2c-450904f3fe8c)

### DryBox Home Assitant Graph

![HA_drybox_graph](https://github.com/user-attachments/assets/67c3691f-dd2e-43b1-ae88-55aabc3eaedd)

![Drybox2_HA](https://github.com/user-attachments/assets/3edbefd8-4533-434f-ae55-5d323bb587a8)
![drybox_ds18b20](https://github.com/user-attachments/assets/d0a6c2c7-14ae-4448-a3f6-f1fff9bb7f26)



# -Un-original-Prusa-Heated-Drybox Code for Ardunio (Nano) 
https://www.printables.com/de/model/883817-unoriginal-prusa-heated-drybox

Parts sold from Blurolls: processor ATmega328P (OldBootloader)

![DryBox](https://github.com/user-attachments/assets/9d7f4efc-d48a-4369-b78b-b287c5fc9cd7)

![Heater_off](https://github.com/user-attachments/assets/19ebee2a-b499-4e50-a4b3-5955bad877ee)
![Heater_on](https://github.com/user-attachments/assets/a007e035-a8de-42e3-872a-942b5bdcf179)

![Heater_offF](https://github.com/user-attachments/assets/02681fed-7551-4cb5-92fd-f77afdf04b4e)
![TargetTempF](https://github.com/user-attachments/assets/f6c2219c-9765-45e8-b5a4-053db0204314)

![AutoOff](https://github.com/user-attachments/assets/677255e3-7602-4d73-b343-3711d502cdc9)
![AutoHum](https://github.com/user-attachments/assets/97c53ffc-8a5c-47c9-a0d7-6cbc97609373)

![With duct](https://github.com/user-attachments/assets/7fc5b59c-0aac-4b53-aea3-175a320b5d3d)
![Drybox_with_duct](https://github.com/user-attachments/assets/e4900aaa-83cf-4a48-a0a8-7c7a8acd7120)

## 2024-12-11: Updated files (Serial_20241211.zip)
 - In heating mode, the update and query interval has been reduced to 2 seconds
 - In addition, the temperature control difference was set to 0.1°C / 0.18°F and the humidity control difference (AutoHum) was set to 0.1% humidity.
 - This results in much faster control behavior (Inspired by Sunlu S4)
 - I am now using a modified duct, provided to me by @wbsp. Changes: 
 - the outlet area at the bottom is 5 mm larger and the outlet at the side is 12 mm higher. 
 - This allows for a much greater air flow. 
![3D Druck_ 20241213_drybox1](https://github.com/user-attachments/assets/e2b94f60-664c-458e-883b-2db02e98c51f)
![3D Druck_ 20241213_drybox_hum](https://github.com/user-attachments/assets/7f5f8fa8-3a3d-4e97-a6f2-916d0916e139)

 ### Comparison with Sunlu S4
 ![Sunlu_S4_Sensoren2](https://github.com/user-attachments/assets/376b4608-f2a4-4173-85eb-7406d9334076)
 ![3D Druck_ 20241213_Sunlu_S4](https://github.com/user-attachments/assets/7c71dd73-0e3e-4558-a1b9-c8586d7f76ef)
 ![3D Druck_ 20241213_Sunlu_S4_1](https://github.com/user-attachments/assets/87c6a775-a6a7-4e7d-83d3-6f022380f5db)
 ![3D Druck_ 20241213_Sunlu_S4_0](https://github.com/user-attachments/assets/6bf77707-35b9-4efd-8d88-690311b0ef35)



## 2024-11-25: Updated files (Serial_20241125.zip)
 With AutoHum, the humidity difference has been reduced to 0.2% (previously 1.0% and 0.5%)
 If the AutoHum function is set below 25%, the TargetTemp is raised to 55°C/131°F, otherwise this humidity is not achieved. 
 The fan run-on time has been slightly reduced at temperatures above 55°C.

Diagram description:
 - At start-up TargetTemp = 50°C, AutoHum = 25%. The front door was still closed. 
 - At around 9:45 the front door was lifted slightly to allow air to circulate. 
 - At 10:30 the AutoHum value was reduced to 20%. Previously, the 25% was reached and regulated
 - After the AutoHum value was not reached at the TargetTemp of 50°C, the TargetTemp was raised to 55°C shortly before 11:00. 
 - With this setting the AutoHum value of 20% was then reached and regulated.
![Drybox_Autohum1](https://github.com/user-attachments/assets/a764c59f-7bc4-4ee7-90cb-de73995ed401)
![Drybox_Autohum2](https://github.com/user-attachments/assets/5d66c4ef-364b-4007-adcc-4243d6b0a0b7)


## 2024-10-15: Updated files (Serial_20241015.zip)
Possibility to switch off the LED strip automatically at 60°C/140°F, 
as the LED strips normally have a maximum operating temperature of 60°C. However, this requires a second Temp/Hum sensor.

## Comparison with/without duct, with/without additional fans
 #### Test environment: 
 - Target temperature 70°C, 
 - AutoOff after 240 minutes (4 hours), 
 - two empty filament rolls in position 2 and position 4.
 - Two front PTFE outlets always open for air exchange. 
 - Overtemperature shutdown of the heater at 90°C, measurement on the floor below the heater/duct
 - Measured with second temp/humidity sensor
 - Graphics from Home Assistant and data from Shelly Plus Add-on and 3 sensors DS18B20 - measuring range -55 °C to +125 °C
   
![Drybox](https://github.com/user-attachments/assets/7c248dd5-6919-43ce-9f6e-be246d9c6c92)

- T1 = Heating temperature
- T2 = Temperature under heating/duct
- T3 = Temperature front right (near door)

### Temperature curve without duct and without additional fans
- The door was lifted slightly after about 1 hour for air exchange and closed again about 5 minutes before the end
- Temperature reached 64.2 °C and 
- temperature under heating 86.9 °C
![Drybox_org_vx](https://github.com/user-attachments/assets/fbe044d5-2b1f-449c-95f4-13a9eeba6cf4)
### Temperature curve with duct and without additional fan
- The door was raised slightly after 5 minutes and was then raised the whole time for air exchange
- Temperature reached 55.8 °C and 
- temperature under duct 73.0 °C
![Drybox_duct_vx](https://github.com/user-attachments/assets/70153dab-5545-4049-8793-a3df9838c742)
### Temperature curve with duct and with two additional fans below duct
- The door was lifted slightly after 5 minutes and was closed again 5 minutes before the end for air exchange
- Temperature reached 60.2 °C and 
- temperature under duct 71.5 °C
![Drybox_fan_vx](https://github.com/user-attachments/assets/ab0df3cf-1e8f-4b73-882e-cd770c9685e8)


## Older test:
#### New: two additional fans below the "duct" for much better air distribution
#### for Bracket Y, Beam A1, Beam A2, Beam A3, Beam A4, duct is now printed with Easy Nylon from Overture.
#### Heat resistance of PC blend is only 113°C/235°F, but over 120°C/248°F is achieved on Bracket Z and duct
#### Easy Nylon has a Heat resistance of 180°C/356°F

![Fan 3and4](https://github.com/user-attachments/assets/1215fead-6184-4725-82ee-8169e7ac9c79)
![Fan 3and4_0](https://github.com/user-attachments/assets/e9687ef8-00eb-48ab-ae89-d33dd17284c8)
![Fan 3and4_1](https://github.com/user-attachments/assets/08f82a89-a02b-481d-9efb-b079f621ada4)
![Additonanl_Fan0](https://github.com/user-attachments/assets/f244d5e3-d652-4fce-857a-0003b4fbc0d1)
![Additional_Fan](https://github.com/user-attachments/assets/a95db112-566f-4278-b561-f9faa1f3ad50)
### New measuring sensors: with Shelly Plus Add-on: DS18B20 - measuring range -55 °C to +125 °C

The second temp/humidity sensor now measures the temperature on the floor under the duct
```
The front door was slightly open
Temperature 2 : Temperature inside duct
Temperature 3 : Temperature Back below duct
Temperature 1 : Temperature front bottom right
Temperature 4 : Temperature rear bottom left
Temperature 5 : Temperature rear bottom right

3D : Temperature/Hum Front bottom right
3DL: Temperature/Hum Back bottom left
3DR: Temperature/Hum Back bottom right
```

### TargetTemp 70°C
![TempGraph_TargetTemp70_1](https://github.com/user-attachments/assets/cb4d5949-33ce-41c2-afa0-20b7bc2dc92f)
![TempGraph_TargetTemp70_2](https://github.com/user-attachments/assets/7bcc4f38-0694-43f9-92ad-76e6c1fb6806)

### TargetTemp 50°C
![TempGraph_TargetTemp50](https://github.com/user-attachments/assets/e6bdaf7e-3aa2-4aec-a2a6-b23405ae19db)
![2024-09-19 16 58 52](https://github.com/user-attachments/assets/d2a88b8f-4c89-4352-bb59-cb04fd0d9a21)


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
![ARDUINO_pin_en](https://github.com/user-attachments/assets/5732b418-ba7c-47e0-9bc3-1c3d006dafbb)


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

#### My recommendation for the filament to be used for "Bracket Y" and "condotto 5" is Nylon 
#### or an even higher temperature resistant filament.
#### In my experience, even PC blend or ASA is not suitable!

### Also important!
    When the heating is on, air exchange must be able to take place, 
    e.g. by lifting the front door or using the unused PTFE outlets, 
    otherwise humidity reduction cannot take place.
![2024-08-29 10 37 31](https://github.com/user-attachments/assets/7d6a6abf-3142-47a8-ad62-016dbb75a41a)
    ![2024-08-29 10 37 23](https://github.com/user-attachments/assets/fd8e3088-1ef1-4410-a763-4399379b0c08)

