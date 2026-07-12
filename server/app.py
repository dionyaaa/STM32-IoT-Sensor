from flask import Flask, request, render_template
from datetime import datetime

app = Flask(__name__)

sensors_data = {
    "temperature": 0.0,
    "pressure": 0.0,
    "humidity": 0.0,
    "time": None
}

@app.route("/", methods = ["GET"])
def index():
    return render_template("index.html", data = sensors_data)

@app.route("/receive", methods = ["POST"])
def receive_data():
    if request.is_json:
        json_data = request.get_json()
        sensors_data["temperature"] = json_data.get("temperature", 0.0)
        sensors_data["pressure"] = json_data.get("pressure", 0.0)
        sensors_data["humidity"] = json_data.get("humidity", 0.0)
        sensors_data["time"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        return {"status": "success"}, 200
    else:
        return {"status": "error"}, 400

if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 5000)