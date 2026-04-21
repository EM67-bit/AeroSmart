#include "config.h"
#include "gps.h"
#include "dht_sensor.h"

void setup() {
  Serial.begin(115200);
  gpsModule.begin();
  dhtSensor.begin();
  Serial.println("Start GPS + DHT11 ...");
}

void loop() {
  // feed GPS continuously for 2 seconds instead of blocking with delay()
  unsigned long start = millis();
  while (millis() - start < 2000) {
    gpsModule.feed();
  }

  GPSData gpsData = gpsModule.read();
  DHTData dhtData = dhtSensor.read();

  if (gpsData.valid) {
    Serial.print("Latitude  : "); Serial.println(gpsData.lat, 6);
    Serial.print("Longitude : "); Serial.println(gpsData.lng, 6);
    Serial.print("Height    : "); Serial.print(gpsData.altitude, 1); Serial.println(" m");
    Serial.print("Satellites: "); Serial.println(gpsData.satellites);
    Serial.print("Precision : "); Serial.print(gpsData.hdop, 2); Serial.println(" HDOP");
    Serial.print("Velocity  : "); Serial.print(gpsData.speed, 1); Serial.println(" km/h");
    Serial.printf("UTC Time  : %02d:%02d:%02d\n", gpsData.hour, gpsData.minute, gpsData.second); //%02d:%02d:%02d\n is a c format that means 0 = with 0 at the front, 2 = min 2 digits, d = a whole number, : prints :, \n = new line
  } else {
    Serial.println("Waiting on GPS ...");
  }

  if (dhtData.valid) {
    Serial.print("Temperature: "); Serial.print(dhtData.temperature); Serial.println(" °C");
    Serial.print("Humidity   : "); Serial.print(dhtData.humidity); Serial.println(" %");
  }
}