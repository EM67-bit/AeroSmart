#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include "config.h"

class BMP390 {
public:
  BMP390();
  bool begin();
  bool read();
  void printData();

  float getPressure();     // hpa
  float getTemperature();  // c
  float getAltitude();     // meters

private:
  Adafruit_BMP3XX _bmp;
  float _pressure;
  float _temperature;
  float _altitude;

  static constexpr float SEA_LEVEL_HPA = 1013.25f;
};
