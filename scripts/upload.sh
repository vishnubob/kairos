#!/bin/sh
avrdude -c arduino -p atmega328p -P ./ports/mcu -b 57600 -D -U flash:w:kairos.ino.hex:i
