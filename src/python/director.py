import os
from . plunger import build_manager
from . controller import build_controller

ARDUINO_PORT = os.getenv("ARDUINO_PORT", "/dev/ttyUSB1")
PLUNGER_PORT = os.getenv("PLUNGER_PORT", "/dev/ttyUSB0")
print("ARDUINO_PORT =", ARDUINO_PORT)
print("PLUNGER_PORT =", PLUNGER_PORT)

class Director(object):
    def __init__(self):
        self.mcu = build_controller(ARDUINO_PORT)
        self.pumps = build_manager(ARDUINO_PORT)

    def init_pumps(self):
        for pump in self.pumps:
            pump.initialize()

    def drip_once(self, pump_idx, step=1):
        pump = self.pumps[pump_idx]
        pump.fill()
        self.mcu.arm(dryrun=True)
        for pos in range(1, 3001, step):
            pump.position = pos
            if self.mcu.triggered:
                break
        return pos
