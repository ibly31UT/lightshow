from flask import render_template, request
from app import app
import json
import serial

#ser = serial.Serial('/dev/cu.usbmodem1421', 19200)
ser = 1

#{"type": "value", "name": "Brightness", "rangelower": 0, "rangeupper": 255}

modes = [
	{"name": "Bounce", "opmode": 0, "controls": [
		{"type": "color", "name": "Color 1"},
		{"type": "color", "name": "Color 2"}]},
	{"name": "Rainbow", "opmode": 1, "controls": [
		{"type": "time", "name": "Speed"}]},
	{"name": "Flow", "opmode": 2, "controls": [
		{"type": "color", "name": "Color"},
		{"type": "time", "name": "Speed"}]},
	{"name": "Flow 2", "opmode": 3, "controls": [
		{"type": "color", "name": "Color"},
		{"type": "time", "name": "Speed"}]},
	{"name": "Random", "opmode": 4, "controls": [
		{"type": "time", "name": "Speed"}]},
	{"name": "Rainbow Flow", "opmode": 5, "controls": [
		{"type": "time", "name": "Speed"}]},
	{"name": "Fill", "opmode": 6, "controls": [
		{"type": "time", "name": "Speed"},
		{"type": "value", "name": "Subdivisions", "rangelower": 0, "rangeupper": 3}]}
]


@app.route("/", methods=["GET", "POST"])
@app.route("/index", methods=["GET", "POST"])
def index():
    return render_template("index.html", title="Home", modes=modes)


@app.route("/playShow", methods=["GET", "POST"])
def playShow():
    requestObject = request.get_json()
    print requestObject
    opmode = requestObject["opmode"]

    message = str(opmode)
    if "time" in requestObject:
        message += "S" + requestObject["time"]
    if "value" in requestObject:
        message += "S" + requestObject["value"]
    if "color" in requestObject:
        message += "S" + requestObject["color"]

    message = message.ljust(20, 'X')
    print message
    print len(message)

    for char in message:
        ser.write(chr(ord(char)))
    return json.dumps({"status": "OK"})