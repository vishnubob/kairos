#!/usr/bin/python

import serial
import time

DEFAULT_PORT = './ports/mcu'

def reset_arduino(port):
    arduino = serial.Serial(port, 115200)
    arduino.setDTR(1)
    time.sleep(.1)
    arduino.setDTR(0)

reset_arduino(DEFAULT_PORT)
