#!/bin/sh
#python arduino_reset.py
avrdude -c arduino -p atmega328p -P ./mcu -b 57600 -D -U flash:w:flashphoto.ino.hex:i
