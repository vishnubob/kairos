#!/bin/sh
python arduino_reset.py
avrdude -c arduino -p atmega328p -P /dev/ttyUSB1 -b 57600 -D -U flash:w:flashphoto.ino.hex:i