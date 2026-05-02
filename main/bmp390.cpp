#include "bmp390.h"

BMP390::BMP390() : _pressure(0), _temperature(0), _altitude(0) {}

bool BMP390::begin() {
  if (!_bmp.begin_I2C(BMP390_I2C_ADDR)) {
    Serial.println("[BMP390] ERROR: Sensor not found. Check wiring/SDO pin.");
    return false;
  }

  // recommended settings for weather monitoring
  _bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  _bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  _bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  _bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("[BMP390] Initialized OK.");
  return true;
}

bool BMP390::read() {
  if (!_bmp.performReading()) {
    Serial.println("[BMP390] ERROR: Failed to perform reading.");
    return false;
  }
  _temperature = _bmp.temperature;
  _pressure    = _bmp.pressure / 100.0f;  // pa  hpa
  _altitude    = _bmp.readAltitude(SEA_LEVEL_HPA);
  return true;
}

void BMP390::printData() {
  Serial.println("--- Altitued/Pressure/Temperature from BMP390 ---");
  Serial.print("  Temperature : "); Serial.print(_temperature, 2); Serial.println(" °C");
  Serial.print("  Pressure    : "); Serial.print(_pressure,    2); Serial.println(" hPa");
  Serial.print("  Altitude    : "); Serial.print(_altitude,    2); Serial.println(" m");
}

float BMP390::getPressure()    { return _pressure;    }
float BMP390::getTemperature() { return _temperature; }
float BMP390::getAltitude()    { return _altitude;    }
