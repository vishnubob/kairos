#!/usr/bin/env python
import time
from timestop import director

#flash_delay = 13073
#flash_delay = 18289 - 3812
#FC = 3750
FC = 4437
delay = 14000

d = director.Director()
pump = d.pumps[0]
while 1:
    print("DELAY", delay)
    print("FLASH DELAY", delay - FC)
    for X in range(1):
        d.mcu.set_flash_delay(delay)
        d.mcu.set_flash_delay(delay - FC)
        d.drip_once(0)
        time.sleep(5)
    FC += 1
    #delay += 100
