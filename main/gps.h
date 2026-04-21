#ifndef GPS_H
#define GPS_H

#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "config.h"

struct GPSData {
  float lat;
  float lng;
  float altitude;
  float speed;
  float hdop;
  int satellites;
  int hour;
  int minute;
  int second;
  bool valid;
};

class GPS {
  public:
    void begin();
    void feed();
    GPSData read();

  private:
    HardwareSerial _serial{1};  // UART1
    TinyGPSPlus _gps;
};

extern GPS gpsModule;

#endif