#!/usr/bin/env python
import sys
import time
import bisect
import pprint
from kairos import director
from kairos import camera

def set_offset(d, offset):
    flash = 1000
    duration = 5000

    d.mcu.set_flash_delay(flash)
    d.mcu.set_flash_duration(flash + duration)
    d.mcu.set_shutter_delay(flash + offset)
    d.mcu.set_shutter_duration(flash + offset + duration)

class BrightnessShim(object):
    def __init__(self):
        self._cache = {}
        self.director = director.Director()
        cam = camera.KairosCamera(0)
        cam.remove_all()

    def scan(self, value=127):
        low = .05
        high = .1
        print(low, high)
        sys.stdout.flush()
        low = int(round((16e6 / 256.0) * low))
        high = int(round((16e6 / 256.0) * high))
        return bisect.bisect_left(self, value, lo=low, hi=high)

    def __getitem__(self, offset):
        if offset not in self._cache:
            self._cache[offset] = self.test(offset)[0]
            print (offset, self._cache[offset])
            sys.stdout.flush()
        pprint.pprint(self._cache)
        return self._cache[offset]

    def test(self, offset):
        while True:
            set_offset(self.director, offset)
            time.sleep(.25)
            self.director.mcu.arm()
            time.sleep(.25)
            self.director.mcu.expose()
            time.sleep(1)
            self.director.mcu.disarm()
            try:
                cam = camera.KairosCamera(0)
                data = cam.download(0)
                ret = camera.lit_pixels(data)
                cam.remove_all()
                time.sleep(5)
                return ret
            except:
                cam = None
                print("failed to expose")
                sys.stdout.flush()
                time.sleep(5)

if __name__ == "__main__":
    bs = BrightnessShim()
    val = bs.scan()
    print("tick", val)
