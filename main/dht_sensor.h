#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include "config.h"

struct DHTData {
  float humidity;
  float temperature;
  bool valid;
};

class DHTSensor {
  public:
    void begin();
    DHTData read();

  private:
    DHT *_dht;
};

extern DHTSensor dhtSensor;

#endif