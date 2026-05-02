from flask import Flask, jsonify, send_from_directory
import sqlite3, os, pathlib

DB  = os.path.join(os.path.dirname(os.path.abspath(__file__)), "aerosmart.db")
DIR = pathlib.Path(__file__).parent

app = Flask(__name__, static_folder=str(DIR), static_url_path="/static")


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


def query(sql, params=()):
    conn = sqlite3.connect(DB)
    conn.row_factory = sqlite3.Row
    rows = conn.execute(sql, params).fetchall()
    conn.close()
    return [dict(r) for r in rows]


@app.route("/api/latest")
def latest():
    rows = query("SELECT * FROM log ORDER BY rowid DESC LIMIT 1")
    return jsonify(rows[0] if rows else {})


@app.route("/api/history")
def history():
    rows = query("SELECT * FROM log ORDER BY rowid DESC LIMIT 200")
    rows.reverse()
    return jsonify(rows)


@app.route("/api/track")
def track():
    rows = query("SELECT time, lat, lon, gps_alt, speed FROM log ORDER BY rowid")
    return jsonify(rows)


# done with claude ai
@app.route("/api/stats")
def stats():
    rows = query("""
        SELECT
            COUNT(*)          AS total_rows,
            MIN(time)         AS first_seen,
            MAX(time)         AS last_seen,
            AVG(dht_temp)     AS avg_temp,
            MIN(dht_temp)     AS min_temp,
            MAX(dht_temp)     AS max_temp,
            AVG(humidity)     AS avg_humidity,
            AVG(pressure)     AS avg_pressure,
            MAX(speed)        AS max_speed,
            MAX(gps_alt)      AS max_alt
        FROM log
    """)
    return jsonify(rows[0] if rows else {})


@app.route("/")
def index():
    return send_from_directory(str(DIR), "dashboard.html")


if __name__ == "__main__":
    init_db()
    app.run(debug=True, port=5000)
