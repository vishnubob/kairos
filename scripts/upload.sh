#!/bin/sh
#python arduino_reset.py
avrdude -c arduino -p atmega328p -P ./ports/mcu -b 57600 -D -U flash:w:timestop.ino.hex:i
