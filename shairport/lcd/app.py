import re
import sys
from utils import start_data, start_item, read_data, guessImageMime
from Adafruit_CharLCD import Adafruit_CharLCD
from time import sleep, strftime
from datetime import datetime


lcd = None
song = None

def send_song():
    print('soy una cancion'+ song["title"] + "\n" + song["artist"]  + " - " + song["album"] )
    lcd.clear()
    lcd.message(song["title"] + "\n" + song["artist"]  + " - " + song["album"])
    return

def send_message(message):

    return

def manage_image(length, data):
    return

def manage_song(code, data):
    print(data)
    if (code == "asal"):
        song["album"] = data
    elif (code == "asar"):
       song["artist"] = data
    elif (code == "minm"):
       song["title"] = data

def manage_type(typ, code, length):
    data = ""
    if (length > 0):
        r = start_data(sys.stdin.readline())
        if (r == -1):
            return
        data = read_data(sys.stdin.readline(), length)
    if (typ == "core"):
        try:
            manage_song(code, str(data,'utf-8'))
        except:
            pass
    if (typ == "ssnc" and code == "PICT"):
        manage_image(length, data)
    if (typ == "ssnc" and code == "mden"):
        send_song()
    if (typ == "ssnc" and code == "pfls"):
        return
    if (typ == "ssnc" and code == "pend"):
        return

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
    song = {}
    lcd = Adafruit_CharLCD(rs=26, en=19,
                       d4=13, d5=6, d6=5, d7=11,
                       cols=16, lines=2)
    lcd.clear()
    lcd.message("prueba")
    fi = sys.stdin
    manager()
