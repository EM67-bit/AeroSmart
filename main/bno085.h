#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "config.h"

class BNO085 {
public:
  BNO085(int8_t rstPin, int8_t intPin);
  bool begin();
  bool read();
  void printData();

  // quaternion unit quaternion
  float getQW(); float getQX(); float getQY(); float getQZ();

  // euler angles derived from quaternion degrees
  float getRoll();  float getPitch(); float getYaw();

  // linear acceleration ms
  float getAccelX(); float getAccelY(); float getAccelZ();

  // gyroscope rads
  float getGyroX(); float getGyroY(); float getGyroZ();

private:
  Adafruit_BNO08x _bno;
  int8_t _rstPin, _intPin;

  sh2_SensorValue_t _sensorValue;

  // stored values
  float _qw, _qx, _qy, _qz;
  float _roll, _pitch, _yaw;
  float _ax, _ay, _az;
  float _gx, _gy, _gz;

  void _setReports();
  void _quaternionToEuler(float qr, float qi, float qj, float qk,
                          float &roll, float &pitch, float &yaw);
};
