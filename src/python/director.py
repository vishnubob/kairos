import time
import os
from . plunger import build_manager
from . controller import build_controller

ARDUINO_PORT = os.getenv("ARDUINO_PORT", "/dev/ttyUSB0")
PLUNGER_PORT = os.getenv("PLUNGER_PORT", "/dev/ttyUSB1")
print("ARDUINO_PORT =", ARDUINO_PORT)
print("PLUNGER_PORT =", PLUNGER_PORT)

class Director(object):
    def __init__(self):
        self.mcu = build_controller(ARDUINO_PORT)
        self.pumps = build_manager(PLUNGER_PORT)
        self.mcu.transport.reset()
        self.init()

    def init(self):
        for pump in self.pumps:
            pump.initialize()

    def drip_once(self, pump_idx):
        pump = self.pumps[pump_idx]
        pos = None
        while pos == None:
            pump.speed = 1
            pump.valve = "input"
            pump.move(3000)
            pump.valve = "output"
            pump.dispense(530)
            pump.speed = 25
            pump.dispense(1000, wait=False)
            #self.mcu.arm(dryrun=True)
            self.mcu.arm()
            while not pump.ready:
                if self.mcu.triggered:
                    pump.stop()
                    pos = pump.position
                    break
            time.sleep(1)
            self.mcu.disarm()
            time.sleep(2)
        return pos

    def set_flashpoint(self, flash):
        self.tm_shutter_on = 1000
        self.tm_shutter_off = 5000
        #self.tm_flash_on = 
        #self.tm_flash_off
        #shutter_delta = 3755 + 20
        #shutter_delta = 5000
        shutter_delta = 3755 - 150

        self.mcu.set_flash_delay(flash)
        self.mcu.set_flash_duration(flash + f_duration)
        self.mcu.set_shutter_delay(flash + shutter_delta)
        self.mcu.set_shutter_duration(flash + shutter_delta + duration)
