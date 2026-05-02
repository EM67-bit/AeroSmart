"""
Arduino serial output simulator.
Writes realistic fake data to a virtual COM port (or named pipe on Windows).

Usage:
    python simulator.py COM5          # pick the OTHER end of a virtual COM pair
    python simulator.py /dev/ttyUSB0

On Windows: install com0com, create a pair e.g. COM4<->COM5.
  Run simulator on COM5, run serial_reader on COM4.

On Linux/Mac: use socat
  socat -d -d pty,raw,echo=0 pty,raw,echo=0
  It prints two /dev/pts/N paths — use one for simulator, one for reader.
"""

import sys, time, math, random, serial

PORT = sys.argv[1] if len(sys.argv) > 1 else "COM5"
BAUD = 115200

# base GPS coords (Zurich)
BASE_LAT =  47.3769
BASE_LON  =  8.5417
BASE_ALT  = 408.0

t = 0

def rand(center, spread):
    return center + random.uniform(-spread, spread)

def gen_frame():
    global t
    t += 1

    lat  = BASE_LAT  + math.sin(t * 0.01) * 0.001
    lon  = BASE_LON  + math.cos(t * 0.01) * 0.001
    alt  = BASE_ALT  + math.sin(t * 0.05) * 15
    sats = random.randint(7, 12)
    hdop = round(random.uniform(0.8, 2.2), 2)
    spd  = abs(math.sin(t * 0.03) * 40)
    hh   = (t // 3600) % 24
    mm   = (t // 60) % 60
    ss   = t % 60

    dht_temp = rand(22.5, 3.0)
    humidity = rand(55.0, 10.0)

    bmp_temp = rand(21.8, 2.5)
    pressure = rand(1013.25, 5.0)
    bmp_alt  = alt + rand(0, 2)

    roll  = rand(0.0, 15.0)
    pitch = rand(0.0, 10.0)
    yaw   = (t * 2) % 360

    ax = rand(0.0, 0.5)
    ay = rand(0.0, 0.5)
    az = rand(9.81, 0.3)
    gx = rand(0.0, 0.05)
    gy = rand(0.0, 0.05)
    gz = rand(0.0, 0.05)

    lines = [
        f"---GPS---",
        f"Latitude  : {lat:.6f}",
        f"Longitude : {lon:.6f}",
        f"Height    : {alt:.1f} m",
        f"Satellites: {sats}",
        f"Precision : {hdop:.2f} HDOP",
        f"Velocity  : {spd:.1f} km/h",
        f"UTC Time  : {hh:02d}:{mm:02d}:{ss:02d}",
        f"---Temp/Humidity from DHT---",
        f"Temperature: {dht_temp:.2f} °C",
        f"Humidity   : {humidity:.2f} %",
        f"--- BMP390 ---",
        f"  Temperature : {bmp_temp:.2f} °C",
        f"  Pressure    : {pressure:.2f} hPa",
        f"  Altitude    : {bmp_alt:.2f} m",
        f"--- BNO085 ---",
        f"  Roll  : {roll:.2f} °",
        f"  Pitch : {pitch:.2f} °",
        f"  Yaw   : {yaw:.2f} °",
        f"  Accel : X={ax:.3f}  Y={ay:.3f}  Z={az:.3f} m/s²",
        f"  Gyro  : X={gx:.3f}  Y={gy:.3f}  Z={gz:.3f} rad/s",
    ]
    return "\r\n".join(lines) + "\r\n"


def main():
    print(f"Simulator -> {PORT} @ {BAUD}")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"Cannot open {PORT}: {e}")
        sys.exit(1)

    print("Sending frames every 2s. Ctrl+C to stop.\n")
    try:
        while True:
            frame = gen_frame()
            ser.write(frame.encode("utf-8"))
            print(frame)
            time.sleep(2)
    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        ser.close()


if __name__ == "__main__":
    main()
