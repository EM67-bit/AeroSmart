#include "gps.h"

GPS gpsModule;

void GPS::begin() {
  _serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void GPS::feed() {
  while (_serial.available() > 0) {
    _gps.encode(_serial.read());
  }
}

GPSData GPS::read() {
  GPSData data;
  data.valid = false;

  if (_gps.location.isValid() && _gps.location.age() < 2000) {
    data.valid = true;
    data.lat = _gps.location.lat();
    data.lng = _gps.location.lng();
    data.altitude = _gps.altitude.meters();
    data.speed = _gps.speed.kmph();
    data.hdop = _gps.hdop.hdop();
    data.satellites = _gps.satellites.value();
    data.hour = _gps.time.hour();
    data.minute = _gps.time.minute();
    data.second = _gps.time.second();
  }

  return data;
}
