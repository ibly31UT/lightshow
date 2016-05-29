import re

code = open("code.li", "r")
codeString = code.read()

p_forby = re.compile(
    "for ([A-Za-z_]+) from ([A-Za-z0-9_.\[\]]+) to ([A-Za-z0-9_.\[\]]+) by ([0-9]+)")
p_for = re.compile(
    "for ([A-Za-z_]+) from ([A-Za-z0-9_.\[\]]+) to ([A-Za-z0-9_.\[\]]+):")
p_if = re.compile("if (.*):")
p_elif = re.compile("elif (.*):")
p_else = re.compile("else:")
p_define = re.compile("([A-Za-z]+) ([A-Za-z_.\[\]]+) = (.*)")
p_assign = re.compile("([A-Za-z_.\[\]]+) = (.*)")
p_func = re.compile("([A-Za-z]+)\((.*)\)")
p_incdec = re.compile("([A-Za-z_.\[\]]+)\s?(\+\+|\-\-)")
p_operate = re.compile("([A-Za-z_.\[\]]+)\s?(\+=|\-=|\*=|/=)\s?(.*)")

patterns = [p_forby, p_for, p_if, p_elif, p_else,
            p_define, p_assign, p_func, p_incdec, p_operate]
patternNames = ["ForBy", "For", "If", "Elif", "Else",
                "Define", "Assign", "Func", "IncDec", "Operate"]
indentRequired = ["ForBy", "For", "If", "Elif", "Else"]


def createLines(codeString):
    lines = []

    for line in codeString.split("\n"):
        indentation = 0
        for char in line:
            if char == "\t" or char == " ":
                indentation += 1
            else:
                break  # read up till first non-tab character
        line = line.replace("\t", "")
        line = line.replace("\t", " ")
        lineObj = {"indent": indentation, "string": line}
        lines.append(lineObj)

    for i, line in enumerate(lines):
        lineStr = line["string"]
        indent = line["indent"]
        for i in range(0, indent):
            lineStr = "\t" + lineStr
        for j, pattern in enumerate(patterns):
            match = pattern.match(line["string"])
            if match:
                groups = []
                for group in match.groups():
                    groups.append(group)
                line["groups"] = groups
                line["type"] = patternNames[j]
        if "type" not in line:
            line["type"] = "Empty"
            line["groups"] = []
    return lines


def matchCurlies(lines, start, expectedIndent):
    startLine = lines[start]
    curlies = []
    indent = expectedIndent - 1

    print "Searching for curlies above line " + str(start) + " on ind level " + str(expectedIndent)
    while indent >= startLine["indent"]:
        i = start
        while i >= 0:
            if lines[i]["type"] in indentRequired and lines[i]["indent"] == indent:
                curlies.append({"line": start, "indent": indent})
                break
            i -= 1
        indent -= 1

    return curlies


def validateLines(lines):
    i = 0
    expectedIndent = 0
    curlies = []

    while i < len(lines):
        line = lines[i]
        if line["type"] != "Empty":
            if line["type"] in indentRequired:
                if line["indent"] == expectedIndent:
                    expectedIndent += 1
                elif line["indent"] == expectedIndent - 1 and line["type"] in ["Elif", "Else"]:
                    print "End current if, begin elif or else"
                    # expectedIndent stays the same
                elif line["indent"] < expectedIndent:
                    print "End current control block"
                    newCurlies = matchCurlies(lines, i, expectedIndent)
                    for curly in newCurlies:
                        curlies.append(curly)
                    expectedIndent = line["indent"]
                else:
                    print "Expected indent " + str(expectedIndent) + " on line " + str(i) + ", actual: " + str(line["indent"])
                    return False
            else:
                if line["indent"] < expectedIndent:
                    newCurlies = matchCurlies(lines, i, expectedIndent)
                    for curly in newCurlies:
                        curlies.append(curly)
                    expectedIndent = line["indent"]
                elif line["indent"] != expectedIndent:
                    print "Expected indent " + str(expectedIndent) + " on line " + str(i) + ", actual: " + str(line["indent"])
                    return False
        i += 1

    newCurlies = matchCurlies(lines, i - 1, 0)
    print "New curly len: ", len(newCurlies)
    for curly in newCurlies:
        curlies.append(curly)

    i = len(curlies) - 1
    while i >= 0:
        curly = curlies[i]
        line = curly["line"]
        indent = curly["indent"]

        print "Curly of indent " + str(indent) + " on line " + str(line)

        newLine = {"type": "EndBlock", "indent": indent, "groups": []}
        lines.insert(line, newLine)
        i -= 1
    return True


def generateCode(lines):
    code = ""
    indentCount = 0
    for line in lines:
        codeLine = ""
        for i in range(0, line["indent"]):
            codeLine = "\t" + codeLine

        groups = line["groups"]
        if line["type"] in ["For", "ForBy"]:
            var = str(groups[0])
            lower = str(groups[1])
            upper = str(groups[2])
            by = str(1)
            compare = "<"

            if upper < lower:
                compare = ">"

            if len(groups) == 4:
                by = str(groups[3])

            codeLine += "for(int " + var + " = " + lower + "; "
            codeLine += var + " " + compare + " " + \
                upper + "; " + var + " += " + by + "){"
        elif line["type"] == "If":
            codeLine += "if(" + groups[0] + "){"
        elif line["type"] == "Elif":
            codeLine += "}else if(" + groups[0] + "){"
        elif line["type"] == "Else":
            codeLine += "}else{"
        elif line["type"] == "Assign":
            codeLine += groups[0] + " = " + groups[1] + ";"
        elif line["type"] == "Define":
            typename = groups[0]
            var = groups[1]
            eq = groups[2]

            if typename == "color":
                typename = "struct rgb_value"
            codeLine += typename + " " + var + " = " + eq + ";"
        elif line["type"] == "Func":
            # check if this function is a global function, transform it
            func = groups[0]
            arguments = groups[1].split(",")

            if func == "setPixel":
                func = "leds.setPixelColor"
                arguments[0] += " + CENTER_OFFSET"
                if len(arguments) == 2:
                    cvar = arguments[1]
                    arguments = [arguments[0], cvar +
                                 ".r", cvar + ".g", cvar + ".b"]
            elif func == "show":
                func = "leds.show"
            elif func == "delay":
                arguments = ["DELAY"]

            codeLine += func + "("
            for i, arg in enumerate(arguments):
                codeLine += arg
                if i < len(arguments) - 1:
                    codeLine += ","
            codeLine += ");"
        elif line["type"] == "IncDec":
            print groups
            codeLine += groups[0] + groups[1] + ";"
        elif line["type"] == "Operate":
            print groups
            codeLine += groups[0] + " " + groups[1] + " " + groups[2] + ";"
        elif line["type"] == "EndBlock":
            codeLine += "}"
        else:
            print "Non valid line"

        if line["type"] in indentRequired:
            indentCount += 1

        codeLine += "\n"
        code += codeLine

    return code


lines = createLines(codeString)
if validateLines(lines):
    generated = open("out.c", "w")
    code = generateCode(lines)
    generated.write(code)
    generated.close()

for i, line in enumerate(lines):
    string = str(i) + ": "
    for j in range(0, line["indent"]):
        string = "\t" + string
    string += line["type"]
    print string
