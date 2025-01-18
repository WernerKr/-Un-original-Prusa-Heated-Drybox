void sensorUpdate(){
  #ifdef DS18B20xx
   TempSensor = TempSensor + 1;
   if (TempSensor > TempSensorMax) {TempSensor = 0; }
  #endif
  
  #ifdef SecondTemp
    Temperature2 = dht.readTemperature() + Temperature2Cor;
    //Temperature_F = dht.readTemperature(true);
   #ifdef Fahrenheit
    Temperature2 = (Temperature2 *9/5) + 32;   // Converts temperature to fahrenheit if defined
    //Temperature2 = 103.3;
   #endif
    Humidity2 = dht.readHumidity() + Humidity2Cor;
   if (Humidity2 == Humidity2) {
     //Good value
    } else {
     Humidity2 = 0;
     Temperature2 = 0;
   }

  if (overHeat == true){
   if (Temperature2 >= HeatMaxValue)
      { TempHigh = true; }
   else
      { 
        TempHigh = false; 
      }
   } 
  #endif  

  Temperature = dht20.getTemperature() + TemperatureCor;      // Get temperature in degrees Celcius
  Humidity = (dht20.getHumidity()*100) + HumidityCor;        // Get relative humidity
  #ifdef Fahrenheit
    Temperature = (Temperature *9/5) + 32;   // Converts temperature to fahrenheit if defined
    //Temperature = 102.4;
  #endif
  if (Humidity >= (99.9 + HumidityCor)){
    Humidity = 0;
    Temperature = 0;
  }

#ifdef DS18B20xx
 sensors.requestTemperatures(); 

 #ifdef DallasSensor1
  #ifndef DallasAdrSet
   sensor1val = sensors.getTempCByIndex(0);
  #endif
  #ifdef DallasAdrSet
   sensor1val = sensors.getTempC(sensor1);
  #endif
  if ((sensor1val <= 0) or (sensor1val == 85.00))  {
    sensor1val  = 0;
  }
    #ifdef Fahrenheit
     if (sensor1val != 0){
     sensor1val = (sensor1val *9/5) + 32;  
     }
    #endif
 #endif
 #ifdef DallasSensor2
  #ifndef DallasAdrSet
   sensor2val = sensors.getTempCByIndex(1);
  #endif
  #ifdef DallasAdrSet
   sensor2val = sensors.getTempC(sensor2);
  #endif
  if ((sensor2val <= 0)  or (sensor2val == 85.00)){
    sensor2val  = 0;
  }  
    #ifdef Fahrenheit
     if (sensor2val != 0){
     sensor2val = (sensor2val *9/5) + 32;  
     }
    #endif
 #endif
 #ifdef DallasSensor3
  #ifndef DallasAdrSet
   sensor3val = sensors.getTempCByIndex(2);
  #endif
  #ifdef DallasAdrSet
   sensor3val = sensors.getTempC(sensor3);
  #endif
  if ((sensor3val <= 0) or (sensor3val == 85.00)){
    sensor3val  = 0;
  }  
    #ifdef Fahrenheit
     if (sensor3val != 0){
     sensor3val = (sensor3val *9/5) + 32;  
     }
    #endif
 #endif

#endif

}

void heater(){
  if (TempHigh == false)
  {
    if(Temperature < (TargetTemp - tempDiff))
    {
      if ((AutoHum == false) or ((AutoHum == true) and (HumOff == false)))
      digitalWrite(Heater, HIGH);            // Turns heating element on if more than 0.2 of 0.5 degrees celcius under the target temperature
      if ((AutoHum == true) and (FanHumOn == true))
      { 
      digitalWrite(Fan, HIGH);
      }
      else { digitalWrite(Fan, HIGH); }
      FanRun = true;
      Hot = true;
    } 
    if ((Temperature > (TargetTemp + tempDiff)) or ((AutoHum == true) and (HumOff == true ))){      // Turns heating element off if more than 0.2 or 0.5 degrees celcius over the target temperature
      digitalWrite(Heater, LOW);
      Hot = false;  
      if ((AutoHum == true) and (FanValue <=0)) {
       digitalWrite(Fan, LOW);
       FanRun = false; 
      }   
    } 
  }
  else { digitalWrite(Heater, LOW); }
}