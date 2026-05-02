#include "bno085.h"
#include "config.h"
#include <math.h>

BNO085::BNO085(int8_t rstPin, int8_t intPin)
  : _bno(rstPin), _rstPin(rstPin), _intPin(intPin),
    _qw(1), _qx(0), _qy(0), _qz(0),
    _roll(0), _pitch(0), _yaw(0),
    _ax(0), _ay(0), _az(0),
    _gx(0), _gy(0), _gz(0) {}

bool BNO085::begin() {
  // hard reset via rst pin before init  ensures the sensors internal
  // power rails are stable before the esp32s3 tries to communicate
  // without this the bno085 sometimes boots into a silent zerooutput state
  pinMode(_rstPin, OUTPUT);
  digitalWrite(_rstPin, LOW);
  delay(10);   // hold reset low for 10ms
  digitalWrite(_rstPin, HIGH);
  delay(100);  // wait 100ms for sensor hub firmware to fully boot

  // retry up to 5 times  the bno085 occasionally needs more than one
  // attempt to respond on i2c after a reset especially on esp32s3
  for (uint8_t attempt = 1; attempt <= 5; attempt++) {
    if (_bno.begin_I2C(BNO085_I2C_ADDR)) {
      delay(50);  // short settle before enabling reports
      _setReports();
      Serial.println("[BNO085] Initialized OK.");
      return true;
    }
    Serial.print("[BNO085] Init attempt ");
    Serial.print(attempt);
    Serial.println("/5 failed, retrying...");
    delay(100);
  }

  Serial.println("[BNO085] ERROR: Sensor not found after 5 attempts. Check wiring/ADR/CS pins.");
  return false;
}

void BNO085::_setReports() {
  // rotation vector fused quaternion at 100 hz
  if (!_bno.enableReport(SH2_ROTATION_VECTOR, 10000)) {
    Serial.println("[BNO085] WARN: Could not enable ROTATION_VECTOR report.");
  }
  // linear acceleration gravity removed at 100 hz
  if (!_bno.enableReport(SH2_LINEAR_ACCELERATION, 10000)) {
    Serial.println("[BNO085] WARN: Could not enable LINEAR_ACCELERATION report.");
  }
  // calibrated gyroscope at 100 hz
  if (!_bno.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000)) {
    Serial.println("[BNO085] WARN: Could not enable GYROSCOPE report.");
  }
}

bool BNO085::read() {
  bool got_data = false;

  // drain available reports  capped at 10 per call to avoid blocking
  // the watchdog timer call this frequently from the main loop instead
  uint8_t count = 0;
  while (count++ < 10 && _bno.getSensorEvent(&_sensorValue)) {
    switch (_sensorValue.sensorId) {
      case SH2_ROTATION_VECTOR:
        _qw = _sensorValue.un.rotationVector.real;
        _qx = _sensorValue.un.rotationVector.i;
        _qy = _sensorValue.un.rotationVector.j;
        _qz = _sensorValue.un.rotationVector.k;
        _quaternionToEuler(_qw, _qx, _qy, _qz, _roll, _pitch, _yaw);
        got_data = true;
        break;
      case SH2_LINEAR_ACCELERATION:
        _ax = _sensorValue.un.linearAcceleration.x;
        _ay = _sensorValue.un.linearAcceleration.y;
        _az = _sensorValue.un.linearAcceleration.z;
        got_data = true;
        break;
      case SH2_GYROSCOPE_CALIBRATED:
        _gx = _sensorValue.un.gyroscope.x;
        _gy = _sensorValue.un.gyroscope.y;
        _gz = _sensorValue.un.gyroscope.z;
        got_data = true;
        break;
    }
  }

  return got_data;
}

void BNO085::printData() {
  Serial.println("--- BNO085 ---");
  Serial.print("  Roll  : "); Serial.print(_roll,  2); Serial.println(" °");
  Serial.print("  Pitch : "); Serial.print(_pitch, 2); Serial.println(" °");
  Serial.print("  Yaw   : "); Serial.print(_yaw,   2); Serial.println(" °");
  Serial.print("  Accel : X="); Serial.print(_ax, 3);
  Serial.print("  Y=");        Serial.print(_ay, 3);
  Serial.print("  Z=");        Serial.print(_az, 3); Serial.println(" m/s²");
  Serial.print("  Gyro  : X="); Serial.print(_gx, 3);
  Serial.print("  Y=");         Serial.print(_gy, 3);
  Serial.print("  Z=");         Serial.print(_gz, 3); Serial.println(" rad/s");
}

//  euler helpers 
void BNO085::_quaternionToEuler(float qr, float qi, float qj, float qk,
                                 float &roll, float &pitch, float &yaw) {
  // zyx aerospace convention  rollpitchyaw in degrees
  float sinr_cosp = 2.0f * (qr * qi + qj * qk);
  float cosr_cosp = 1.0f - 2.0f * (qi * qi + qj * qj);
  roll  = atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;

  float sinp = 2.0f * (qr * qj - qk * qi);
  if (fabsf(sinp) >= 1.0f)
    pitch = copysignf(90.0f, sinp);
  else
    pitch = asinf(sinp) * RAD_TO_DEG;

  float siny_cosp = 2.0f * (qr * qk + qi * qj);
  float cosy_cosp = 1.0f - 2.0f * (qj * qj + qk * qk);
  yaw   = atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;
}

//  getters 
float BNO085::getQW() { return _qw; } float BNO085::getQX() { return _qx; }
float BNO085::getQY() { return _qy; } float BNO085::getQZ() { return _qz; }
float BNO085::getRoll()  { return _roll;  }
float BNO085::getPitch() { return _pitch; }
float BNO085::getYaw()   { return _yaw;   }
float BNO085::getAccelX() { return _ax; } float BNO085::getAccelY() { return _ay; } float BNO085::getAccelZ() { return _az; }
float BNO085::getGyroX()  { return _gx; } float BNO085::getGyroY()  { return _gy; } float BNO085::getGyroZ()  { return _gz; }
