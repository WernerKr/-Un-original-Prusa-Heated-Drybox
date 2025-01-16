void mqtt(){
  _mqtt_remote.handle();

  auto connected = _mqtt_remote.connected();
  if (!_was_connected && connected) {
    // Publish Home Assistant Configuration for the sensors once connected to MQTT.
 
    _ha_entity_mode.publishConfiguration();
    _ha_entity_modetext.publishConfiguration();

    _ha_entity_switch_heater.publishConfiguration();
    _ha_entity_switch_fan.publishConfiguration();
    #ifdef controlLed
     _ha_entity_switch_led.publishConfiguration();
    #endif
    _ha_entity_temperature_target.publishConfiguration();
    _ha_entity_humidity_target.publishConfiguration();
    _ha_entity_temperature.publishConfiguration();
    _ha_entity_humidity.publishConfiguration();

  #ifdef SecondTemp
    _ha_entity_temperature_2.publishConfiguration();
    _ha_entity_humidity_2.publishConfiguration();
  #endif

 #ifdef DS18B20xx

  #ifdef DS18B20HA
   #ifdef DallasSensor1
    _ha_entity_temperature_e1.publishConfiguration();
   #endif 
   #ifdef DallasSensor2
    _ha_entity_temperature_e2.publishConfiguration();
   #endif
   #ifdef DallasSensor3
    _ha_entity_temperature_e3.publishConfiguration();
   #endif
  #endif

  #endif


  }

  _was_connected = connected;

  // Publish temperature and brightness status every 10 seconds.
  auto now = millis();
  if (now - _last_publish_ms >= 10000) {

    if (status == false)
    {   
     _ha_entity_mode.publishNumber(0);
     _ha_entity_modetext.publishString("Off", {{"attr1", "Off"}});
    }
    else
    {
    if (AutoOff == true) 
    { 
     _ha_entity_mode.publishNumber(2);
     char s1[25]="AutoOff ";
     dtostrf(AutoOffTimeValue, 4, 0, MinuteDisplay);
     strcat(s1,MinuteDisplay);
     strcat(s1,"min");     
     _ha_entity_modetext.publishString(s1, {{"attr1", "AutoOff"}, {"attr2", AutoOffTimeValue}});  //AutoOffTimeValue
    }
    else
    if (AutoHum == true) 
    { 
     _ha_entity_mode.publishNumber(3);
     char s1[20]="AutoHum ";     
     dtostrf(AutoHumValue, 2, 0, MinuteDisplay);
     strcat(s1,MinuteDisplay); 
     strcat(s1,"%");    
     _ha_entity_modetext.publishString(s1, {{"attr1", "AutoHum"}});
    }
    else
    { 
     _ha_entity_mode.publishNumber(1);
     char s1[20]="On ";     
     dtostrf(TargetTemp, 2, 0, MinuteDisplay);
     strcat(s1,MinuteDisplay); 
     strcat(s1,text);    
     _ha_entity_modetext.publishString(s1, {{"attr1", "On"}});
    }
  } 

    _ha_entity_switch_heater.publishSwitch(Hot);
    _ha_entity_switch_fan.publishSwitch(FanRun);
    
    #ifdef controlLed
    _ha_entity_switch_led.publishSwitch(LedOn);
    #endif

    _ha_entity_temperature_target.publishTemperature(TargetTemp);
    _ha_entity_humidity_target.publishHumidity(AutoHumValue);
    if ((Humidity >0)) {
     _ha_entity_temperature.publishTemperature(Temperature);
     _ha_entity_humidity.publishHumidity(Humidity);
    }

  #ifdef SecondTemp
    if ((Humidity2 >0)) {
     _ha_entity_temperature_2.publishTemperature(Temperature2);
     _ha_entity_humidity_2.publishHumidity(Humidity2);
    }
  #endif 

  #ifdef DS18B20xx

  #ifdef DS18B20HA
   #ifdef DallasSensor1
    if ((sensor1val >0)) { _ha_entity_temperature_e1.publishTemperature(sensor1val); }
   #endif 
   #ifdef DallasSensor2
    if ((sensor2val >0)) { _ha_entity_temperature_e2.publishTemperature(sensor2val); }
   #endif
   #ifdef DallasSensor3
    if ((sensor3val >0)) { _ha_entity_temperature_e3.publishTemperature(sensor3val); }
   #endif
  #endif

  #endif
    _last_publish_ms = now;
  }
}