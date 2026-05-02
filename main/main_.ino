#include <Wire.h>
#include "config.h"
#include "gps.h"
#include "dht_sensor.h"
#include "bmp390.h"
#include "bno085.h"

BMP390 bmp;
BNO085 bno(BNO_RST_PIN, BNO_INT_PIN);

bool bmpOK = false;
bool bnoOK = false;

void setup() {
  Serial.begin(115200);

  // start i2c on custom pins for bmp390  bno085
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(400000);  // 400 khz fast mode

  gpsModule.begin();
  dhtSensor.begin();

  bmpOK = bmp.begin();
  bnoOK = bno.begin();

  Serial.println("Start GPS + DHT11 + BMP390 + BNO085 ...");
}

void loop() {
  // feed gps continuously for 2 seconds instead of blocking with delay
  unsigned long start = millis();
  while (millis() - start < 2000) {
    gpsModule.feed();
    if (bnoOK) bno.read();  // keep draining bno085 fifo at full rate during gps feed
    yield();                // reset watchdog timer  prevents guru meditation crash
  }

  GPSData gpsData = gpsModule.read();
  DHTData dhtData = dhtSensor.read();

  if (gpsData.valid) {
    Serial.println("--- GPS ---");
    Serial.print("Latitude  : "); Serial.println(gpsData.lat, 6);
    Serial.print("Longitude : "); Serial.println(gpsData.lng, 6);
    Serial.print("Height    : "); Serial.print(gpsData.altitude, 1); Serial.println(" m");
    Serial.print("Satellites: "); Serial.println(gpsData.satellites);
    Serial.print("Precision : "); Serial.print(gpsData.hdop, 2); Serial.println(" HDOP");
    Serial.print("Velocity  : "); Serial.print(gpsData.speed, 1); Serial.println(" km/h");
    Serial.printf("UTC Time  : %02d:%02d:%02d\n", gpsData.hour, gpsData.minute, gpsData.second); // 02d02d02dn is a c format that means 0  with 0 at the front 2  min 2 digits d  a whole number  prints  n  new line
  } else {
    Serial.println("Waiting on GPS ...");
  }

  if (dhtData.valid) {
    Serial.println("--- Humimdity and Temperature from DHT ---");
    Serial.print("Temperature: "); Serial.print(dhtData.temperature); Serial.println(" °C");
    Serial.print("Humidity   : "); Serial.print(dhtData.humidity); Serial.println(" %");
  }

  if (bmpOK) {
    if (bmp.read()) {
      bmp.printData();
    }
  }

  if (bnoOK) {
    bno.printData();
  }
}
