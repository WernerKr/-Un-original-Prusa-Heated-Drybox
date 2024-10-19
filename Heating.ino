void sensorUpdate(){
  #ifdef SecondTemp

    Humidity2 = dht.readHumidity() + Humidity2Cor;
    Temperature2 = dht.readTemperature() + Temperature2Cor;
    //Temperature_F = dht.readTemperature(true);
   #ifdef Fahrenheit
    Temperature2 = (Temperature2 *9/5) + 32;   // Converts temperature to fahrenheit if defined
    //Temperature2 = 103.3;
   #endif

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
}

void heater(){
  if (TempHigh == false)
  {
    if(Temperature < (TargetTemp - tempDiff))
    {
      if ((AutoHum == false) or ((AutoHum == true) and (HumOff == false)))
      digitalWrite(Heater, HIGH);            // Turns heating element on if more than 0.2 of 0.5 degrees celcius under the target temperature
      digitalWrite(Fan, HIGH);
      Hot = true;
    } 
    if ((Temperature > (TargetTemp + tempDiff)) or ((AutoHum == true) and (HumOff == true ))){      // Turns heating element off if more than 0.2 or 0.5 degrees celcius over the target temperature
      digitalWrite(Heater, LOW);
      Hot = false;      
    } 
  }
  else { digitalWrite(Heater, LOW); }
}