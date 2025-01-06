void mqtt(){
  _mqtt_remote.handle();

  auto connected = _mqtt_remote.connected();
  if (!_was_connected && connected) {
    // Publish Home Assistant Configuration for the sensors once connected to MQTT.
 
    _ha_entity_mode.publishConfiguration();

    _ha_entity_switch_heater.publishConfiguration();
    _ha_entity_switch_fan.publishConfiguration();
    _ha_entity_switch_led.publishConfiguration();

    _ha_entity_temperature_target.publishConfiguration();
    _ha_entity_humidity_target.publishConfiguration();
    _ha_entity_temperature.publishConfiguration();
    _ha_entity_humidity.publishConfiguration();

  #ifdef SecondTemp
    _ha_entity_temperature_2.publishConfiguration();
    _ha_entity_humidity_2.publishConfiguration();
  #endif  
  }

  _was_connected = connected;

  // Publish temperature and brightness status every 10 seconds.
  auto now = millis();
  if (now - _last_publish_ms >= 10000) {

    if (status == false)
    {   
    _ha_entity_mode.publishNumber(0);
    }
    else
    {
    if (AutoOff == true) { _ha_entity_mode.publishNumber(2); }
    else
    if (AutoHum == true) { _ha_entity_mode.publishNumber(3); }
    else
    { _ha_entity_mode.publishNumber(1); }
    }

    _ha_entity_switch_heater.publishSwitch(Hot);
    _ha_entity_switch_fan.publishSwitch(FanOn);
    _ha_entity_switch_led.publishSwitch(LedOn);

    _ha_entity_temperature_target.publishTemperature(TargetTemp);
    _ha_entity_humidity_target.publishHumidity(AutoHumValue);
    _ha_entity_temperature.publishTemperature(Temperature);
    _ha_entity_humidity.publishHumidity(Humidity);
  
  #ifdef SecondTemp
    _ha_entity_temperature_2.publishTemperature(Temperature2);
    _ha_entity_humidity_2.publishHumidity(Humidity2);
  #endif  

    _last_publish_ms = now;
  }
}