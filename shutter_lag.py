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

    def scan(self, value, count):
        for x in range(count):
            a = self[x + value]
        pprint.pprint(self._cache)
        return None

    def bisect(self, value=254):
        mid = 0.0613
        low = mid - .01
        high = mid + .03
        print("High:", high, "Low:", low)
        sys.stdout.flush()
        low = int(round((16e6 / 256.0) * low))
        high = int(round((16e6 / 256.0) * high))
        val = bisect.bisect_right(self, value, lo=low, hi=high)
        pprint.pprint(self._cache)
        return val

    def __getitem__(self, offset):
        if offset not in self._cache:
            self._cache[offset] = self.test(offset)[0]
            print (offset, "%.6fms" % (offset / (16e6 / 256.0)), self._cache[offset])
            sys.stdout.flush()
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
                time.sleep(3)

if __name__ == "__main__":
    bs = BrightnessShim()
    #bs.scan(3821, 20)
    val = bs.bisect()
    print("tick", val)
