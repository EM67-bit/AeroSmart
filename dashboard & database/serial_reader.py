import re, sqlite3, sys, os
from datetime import datetime

PORT = sys.argv[1] if len(sys.argv) > 1 else None
BAUD = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
DB   = os.path.join(os.path.dirname(os.path.abspath(__file__)), "aerosmart.db")


def init_db():
    conn = sqlite3.connect(DB)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS log (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            time        TEXT,
            lat         REAL,
            lon         REAL,
            gps_alt     REAL,
            speed       REAL,
            satellites  INTEGER,
            hdop        REAL,
            utc_time    TEXT,
            dht_temp    REAL,
            humidity    REAL,
            bmp_temp    REAL,
            pressure    REAL,
            bmp_alt     REAL,
            roll        REAL,
            pitch       REAL,
            yaw         REAL,
            ax          REAL,
            ay          REAL,
            az          REAL,
            gx          REAL,
            gy          REAL,
            gz          REAL
        )
    """)
    conn.commit()
    conn.close()


def insert_row(row: dict):
    if not row.get("lat"):
        return
    row["time"] = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%S")
    cols         = ", ".join(row.keys())
    placeholders = ", ".join("?" for _ in row)
    conn = sqlite3.connect(DB)
    conn.execute(f"INSERT INTO log ({cols}) VALUES ({placeholders})", list(row.values()))
    conn.commit()
    conn.close()


def _f(s):
    """strip units, parse float"""
    s = re.sub(r"[^\d.\-].*", "", s.strip())
    try: return float(s)
    except: return None


# done with claude ai
def parse_frame(lines):
    """
    Parses one complete frame from Arduino serial output.

    Expected sections and exact line formats:
      ---GPS---
        Latitude  : <val>
        Longitude : <val>
        Height    : <val> m
        Satellites: <val>
        Precision : <val> HDOP
        Velocity  : <val> km/h
        UTC Time  : HH:MM:SS
      ---Temp/Humidity from DHT---
        Temperature: <val> °C
        Humidity   : <val> %
      --- BMP390 ---
        Temperature : <val> °C
        Pressure    : <val> hPa
        Altitude    : <val> m
      --- BNO085 ---
        Roll  : <val> °
        Pitch : <val> °
        Yaw   : <val> °
        Accel : X=<val>  Y=<val>  Z=<val> m/s²
        Gyro  : X=<val>  Y=<val>  Z=<val> rad/s
    """
    row     = {}
    section = None

    for raw in lines:
        line = raw.strip()
        if not line:
            continue

        # section headers
        if "---GPS---" in line:
            section = "gps"
            continue
        if "Temp" in line and "DHT" in line:
            section = "dht"
            continue
        if "BMP390" in line:
            section = "bmp"
            continue
        if "BNO085" in line:
            section = "bno"
            continue
        if "Waiting on GPS" in line:
            section = None
            continue

        # GPS fields
        if section == "gps":
            if "Latitude" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["lat"] = float(m.group(1))
            elif "Longitude" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["lon"] = float(m.group(1))
            elif "Height" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["gps_alt"] = float(m.group(1))
            elif "Satellites" in line:
                m = re.search(r":\s*(\d+)", line)
                if m: row["satellites"] = int(m.group(1))
            elif "Precision" in line:
                m = re.search(r":\s*([\d.]+)", line)
                if m: row["hdop"] = float(m.group(1))
            elif "Velocity" in line:
                m = re.search(r":\s*([\d.]+)", line)
                if m: row["speed"] = float(m.group(1))
            elif "UTC Time" in line:
                m = re.search(r":\s*(\d{2}:\d{2}:\d{2})", line)
                if m: row["utc_time"] = m.group(1)

        # DHT fields
        elif section == "dht":
            if "Temperature" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["dht_temp"] = float(m.group(1))
            elif "Humidity" in line:
                m = re.search(r":\s*([\d.]+)", line)
                if m: row["humidity"] = float(m.group(1))

        # BMP390 fields
        elif section == "bmp":
            if "Temperature" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["bmp_temp"] = float(m.group(1))
            elif "Pressure" in line:
                m = re.search(r":\s*([\d.]+)", line)
                if m: row["pressure"] = float(m.group(1))
            elif "Altitude" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["bmp_alt"] = float(m.group(1))

        # BNO085 fields
        elif section == "bno":
            if "Roll" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["roll"] = float(m.group(1))
            elif "Pitch" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["pitch"] = float(m.group(1))
            elif "Yaw" in line:
                m = re.search(r":\s*([-\d.]+)", line)
                if m: row["yaw"] = float(m.group(1))
            elif "Accel" in line:
                # X=<val>  Y=<val>  Z=<val>
                mx = re.search(r"X=\s*([-\d.]+)", line)
                my = re.search(r"Y=\s*([-\d.]+)", line)
                mz = re.search(r"Z=\s*([-\d.]+)", line)
                if mx: row["ax"] = float(mx.group(1))
                if my: row["ay"] = float(my.group(1))
                if mz: row["az"] = float(mz.group(1))
            elif "Gyro" in line:
                mx = re.search(r"X=\s*([-\d.]+)", line)
                my = re.search(r"Y=\s*([-\d.]+)", line)
                mz = re.search(r"Z=\s*([-\d.]+)", line)
                if mx: row["gx"] = float(mx.group(1))
                if my: row["gy"] = float(my.group(1))
                if mz: row["gz"] = float(mz.group(1))

    return row


def main():
    import serial as pyserial

    init_db()
    port = PORT or "COM3"
    ser  = pyserial.Serial(port, BAUD, timeout=2)

    # frame buffer — collects lines between GPS headers
    frame_lines = []

    while True:
        raw = ser.readline()
        if not raw:
            continue

        line = raw.decode("utf-8", errors="replace").rstrip("\r\n")

        if "---GPS---" in line:
            if frame_lines:
                row = parse_frame(frame_lines)
                if row.get("lat"):
                    insert_row(row)
                    print(f"[OK] lat={row.get('lat')} lon={row.get('lon')} "
                          f"temp={row.get('dht_temp')} roll={row.get('roll')}")
            frame_lines = []

        frame_lines.append(line)


if __name__ == "__main__":
    main()
