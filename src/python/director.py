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
        #self.mcu.transport.reset()

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
            pump.speed = 11
            pump.valve = "output"
            pump.dispense(3000, wait=False)
            #self.mcu.arm(dryrun=True)
            self.mcu.arm()
            time.sleep(.5)
            while not pump.ready:
                if self.mcu.triggered:
                    pump.stop()
                    pos = pump.position
                    break
            time.sleep(2)
            self.mcu.disarm()
        return pos
