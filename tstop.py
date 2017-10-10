#!/usr/bin/env python
import time
from timestop import director

#flash_delay = 13073
#flash_delay = 18289 - 3812
flash_delay = 14500

d = director.Director()
pump = d.pumps[0]
while 1:
    print("FLASH DELAY", flash_delay)
    for X in range(1):
        d.mcu.set_flash_delay(flash_delay)
        d.drip_once(0)
        time.sleep(5)
    flash_delay += 10
