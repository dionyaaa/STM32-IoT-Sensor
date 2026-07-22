from flask import Flask, render_template, request
import ssl
import json
import sqlite3
from datetime import datetime

app = Flask(__name__)

with open("sensors.json", "r", encoding = "utf-8") as file:
    ALLOWED_SENSORS = json.load(file)

def database_init():
    connection = sqlite3.connect("database.db")
    cursor = connection.cursor()
    cursor.execute('''
                   CREATE TABLE IF NOT EXISTS sensor_data(
                   id INTEGER PRIMARY KEY AUTOINCREMENT,
                   sensor_id TEXT NOT NULL,
                   sensor_name TEXT NOT NULL,
                   temperature REAL NOT NULL,
                   pressure REAL NOT NULL,
                   humidity REAL NOT NULL,
                   time TEXT NOT NULL);
    ''')
    connection.commit()
    connection.close()

database_init()

@app.route("/", methods = ["GET"])
def index():
    sensor_names = list(ALLOWED_SENSORS.values())
    return render_template("index.html", names = sensor_names)

@app.route("/database", methods = ["GET"])
def database():
    connection = sqlite3.connect("database.db")
    cursor = connection.cursor()
    cursor.execute("SELECT * FROM sensor_data")
    sensor_data = cursor.fetchall()
    connection.close()
    return render_template("database.html", data = sensor_data)

@app.route("/receive", methods = ["POST"])
def receive():
    sensor_id = request.headers.get("X-Sensor-ID")
    if sensor_id not in ALLOWED_SENSORS:
        return {"status": "error"}, 403
    
    if request.is_json:
        json_data = request.get_json()

        sensor_name = ALLOWED_SENSORS[sensor_id]
        temperature = json_data.get("temperature", 0.0)
        pressure = json_data.get("pressure", 0.0)
        humidity = json_data.get("humidity", 0.0)
        time = datetime.now().strftime("%H:%M:%S %d-%m-%Y")

        connection = sqlite3.connect("database.db")
        cursor = connection.cursor()
        cursor.execute('''INSERT INTO sensor_data(sensor_id, sensor_name, temperature, pressure, humidity, time)
                       VALUES(?, ?, ?, ?, ?, ?)''', (sensor_id, sensor_name, temperature, pressure, humidity, time))
        connection.commit()
        connection.close()

        return {"status": "success"}, 200
    else:
        return {"status": "error"}, 400

if __name__ == "__main__":
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain("certs/cert.pem", "certs/key.pem")
    context.set_ciphers("AES128-SHA:AES256-SHA:DEFAULT:@SECLEVEL=0")

    context.minimum_version = ssl.TLSVersion.TLSv1
    context.maximum_version = ssl.TLSVersion.TLSv1_2

    app.run(host = "0.0.0.0", port = 443, ssl_context = context)