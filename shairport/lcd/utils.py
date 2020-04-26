import re, sys
import base64

def start_item(line):
    regex = r"<item><type>(([A-Fa-f0-9]{2}){4})</type><code>(([A-Fa-f0-9]{2}){4})</code><length>(\d*)</length>"
    matches = re.findall(regex, line)
    typ = bytes.fromhex(matches[0][0]).decode('utf-8')
    code = bytes.fromhex(matches[0][2]).decode('utf-8')
    length = int(matches[0][4])
    return (typ, code, length)

def start_data(line):
    try:
        assert line == '<data encoding="base64">\n'
    except AssertionError:
        if line.startswith("<data"):
            return 0
        return -1
    return 0

def read_data(line, length):
    line = line.replace("\n", "").replace("</item>", "").replace("</data>", "");
    data = ""
    try:
         data = base64.b64decode(line)
    except Exception as e:
        print(e)
        data = ""
    return data

def guessImageMime(magic):
    if magic.startswith('\xff\xd8'):
        return 'image/jpeg'
    elif magic.startswith('\x89PNG\r\n\x1a\r'):
        return 'image/png'
    else:
        return "image/jpg"