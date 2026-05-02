#ifndef CONFIG_H
#define CONFIG_H

// gps
#define GPS_RX_PIN 4
#define GPS_TX_PIN 5
#define GPS_BAUD   9600

// dht11
#define DHTPIN  6
#define DHTTYPE DHT11

// i2c shared by bmp390 and bno085
#define PIN_SDA 1
#define PIN_SCL 2

// bmp390
#define BMP390_I2C_ADDR 0x77  // sdo unconnected  0x77 sdo to gnd  0x76

// bno085
#define BNO085_I2C_ADDR 0x4A  // adr to gnd  0x4a adr to 33v  0x4b
#define BNO_RST_PIN     3
#define BNO_INT_PIN     7

#endif
