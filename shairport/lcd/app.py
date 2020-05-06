import re
import sys
from utils import start_data, start_item, read_data, guess_image_mime, get_string_from_bytes
from Adafruit_CharLCD import Adafruit_CharLCD
from json import dumps
from DisplayManager import display_manager

song = {}
device_info = {}
config = {}
th_display = None

def send_song():
    res = {'song' : song, 'device' : device_info}
    print(dumps(res))
    send_message(song["title"] + " - " + song["artist"]  + " - " + song["album"])
    return

def send_message(message):
    message += " "
    th_display.pause()
    th_display.message = message
    th_display.resume()
    
def manage_image(length, data):
    print('image')
    return

def manage_volume(data):
    device_info["vol"] = data
    print(device_info)

def manage_song(code, data):
    if (code == "asal"):
        song["album"] = data
    elif (code == "asar"):
       song["artist"] = data
    elif (code == "minm"):
       song["title"] = data
    elif (code == "asgn"):
        song["genere"] = data
    elif (code == "ascp"):
        song["author"] = data

def manage_shair(code, data):
    if(code == "mden"):
        send_song()
    elif(code =="snam"):
        device_info["snam"] = data
    elif(code =="acre"):
        device_info["acre"] = data
    elif(code =="dapo"):
        device_info["dapo"] = data
    elif(code =="daid"):
        device_info["daid"] = data
    elif(code == "pvol"):
        manage_volume(data)
    elif(code == "pend"):
        device_info["satus"] = 'END'
    elif(code == "pfls"):
        device_info["satus"] = 'FLUSH'
    elif(code == "prsm"):
        device_info["satus"] = 'RESUME'
    elif(code == "pbeg"):
        device_info["satus"] = 'BEGIN'
    elif(code == "prgr"):
        song["progress"] = data
    elif(code == "pcst"):
        song["start"] = data
    elif(code == "pcen"):
        song["end"] = data
    else:
        config[code] = data
        print(config)
    
def manage_type(typ, code, length):
    data = ""
    if (length > 0):
        r = start_data(sys.stdin.readline())
        if (r == -1):
            return
        data = read_data(sys.stdin.readline(), length)
    if (typ == "core"):
        manage_song(code, get_string_from_bytes(data))
    elif (typ == "ssnc"):
        if(code == "PICT"): manage_image(length, data)
        else: manage_shair(code, get_string_from_bytes(data))

def manager():
    while True:
        line = sys.stdin.readline()
        if not line:  # EOF
            break
        sys.stdout.flush()
        if not line.startswith("<item>"):
            continue
        typ, code, length = start_item(line)
        manage_type(typ, code, length)
        sys.stdout.flush()


if __name__ == "__main__":
    try:
        song = {}
        lcd = Adafruit_CharLCD(rs=26, en=19,
                        d4=13, d5=6, d6=5, d7=11,
                        cols=16, lines=2)
        th_display = display_manager(lcd)
        th_display.start()
        fi = sys.stdin
        manager()
    except KeyboardInterrupt:
        th_display.raise_exception()
        th_display.join()
