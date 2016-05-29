import json
# import serial
from flask import render_template, request
from flask.views import MethodView
from models import LightScript, GridLayout, Selector
from extensions import DB


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


def isMobile(req):
    userAgent = req.headers.get("User-Agent")
    if "mobile" in userAgent.lower():
        return True
    return False


class IndexView(MethodView):

    def get(self):
        return render_template("index.html", title="Home", modes=modes)

    def post(self):
        return render_template("index.html", title="Home", modes=modes)


class PlayView(MethodView):

    def post(self):
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


class GridView(MethodView):

    def get(self):
        scripts = LightScript.query.all()
        selectors = Selector.query.all()
        return render_template("grid.html", title="grid", scripts=scripts,
                               selectors=selectors, mobile=isMobile(request))


class GridLayoutView(MethodView):

    def post(self):
        scripts = request.get_json()
        if GridLayout.query.filter_by(user=3).first() is not None:
            GridLayout.query.filter_by(user=3).update(dict(scripts=json.dumps(scripts)))
        else:
            gridLayout = GridLayout(user=3, scripts=json.dumps(scripts))
            DB.session.add(gridLayout)
        DB.session.commit()
        return json.dumps({"status": "OK"})

    def get(self):
        layout = GridLayout.query.filter_by(user=3).first()
        layoutObj = {"user": layout.user, "scripts": layout.scripts}

        return json.dumps(layoutObj)


class ScriptsView(MethodView):

    def get(self):
        scripts = LightScript.query.all()
        selectors = Selector.query.all()
        scriptList = []
        selectorList = []
        for script in scripts:
            selectorIds = []
            for selector in script.selectors:
                selectorIds.append(selector.id)
            scriptList.append({"id": script.id, "name": script.name, "script": script.script, "selectors": selectorIds, "color": script.color})
        for selector in selectors:
            selectorList.append({"id": selector.id, "name": selector.name, "html": selector.html, "js": selector.js})
        return json.dumps({"scripts": scriptList, "selectors": selectorList})
