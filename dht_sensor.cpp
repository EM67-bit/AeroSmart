#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include "dht_sensor.h"

DHTSensor dhtSensor;

void DHTSensor::begin() {
  _dht = new DHT(DHTPIN, DHTTYPE);
  _dht->begin();
}

DHTData DHTSensor::read() {
  DHTData data;
  data.valid = false;

  data.humidity = _dht->readHumidity();
  data.temperature = _dht->readTemperature();

  if (isnan(data.humidity) || isnan(data.temperature)) { //isnan() checks if the value isn't a number, if it has no number it has no data
    Serial.println("No DHT11 data!");
    return data;
  }

  data.valid = true;
  return data;
}