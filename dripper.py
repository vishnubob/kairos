#!/usr/bin/env python
import time
from kairos import director

class Frontend(object):
    def __init__(self):
        self.ctrl = director.Director()
        self.mcu = self.ctrl.mcu
        self.pump = self.ctrl.pumps[0]
        self.exp = 10000
        self.delta = 4975
        self.mirror = False

    def shutter(self):
        if self.mirror:
            self.mcu.send('x')
        self.mcu.send('X')
        time.sleep(1)

    def movie(self, steps):
        steps = range(*steps)
        for delay in steps:
            print("DELAY", delay)
            self.exp = delay
            self.drip()

    def parse_line(self, cmd):
        if cmd[0].lower() == 'k':
            self.mcu.mirror_lockup()
            self.mirror = not self.mirror
            if self.mirror:
                print("Mirror locked up")
            else:
                print("Mirror not locked up")
        elif cmd[0].lower() == 'd':
            self.delta = int(cmd[1:])
            print("delta set")
        elif cmd[0].lower() == 'x':
            self.mcu.send('x')
            print("shuttered")
        elif cmd[0].lower() == 'g':
            if self.mirror:
                self.mcu.send('x')
            self.mcu.send('X')
            print("shuttered")
        elif cmd[0].lower() == 'm':
            args = cmd[1:].split(',')
            if len(args) == 1:
                args = (self.exp, args[0])
            elif len(args) == 2:
                args = (self.exp, args[0], args[1])
            args = tuple(map(int, args))
            print("movie", args)
            self.movie(args)
        else:
            self.exp = int(cmd)
            self.drip()

    def drip(self):
        duration = 3750
        self.mcu.set_flash_delay(self.exp)
        self.mcu.set_flash_duration(self.exp + duration)
        self.mcu.set_shutter_delay(self.exp - self.delta)
        self.mcu.set_shutter_duration(self.exp - self.delta + duration)
        self.ctrl.drip_once(0)

    def prompt(self):
        print("")
        while 1:
            print("[ exp:", self.exp, "], [ delta:", self.delta, "]")
            cmd = input("cmd> " )
            if cmd:
                self.parse_line(cmd)
            else:
                self.drip()

t = Frontend()
t.prompt()
