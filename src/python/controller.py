import serial
import time
from . transport import SerialTransport

class ControllerTransport(SerialTransport):
    DefaultBaudrate = 115200

class ControllerError(Exception):
    pass

class Controller(object):
    def __init__(self, transport=None):
        self.transport = transport
        self.cache = {}

    def send(self, cmd, callback=None):
        def wrap_callback(plunger, callback):
            def wrapper(resp):
                self.cache["status"] = status = resp.strip()
                if callback:
                    value = callback(resp)
            return wrapper
        callback = wrap_callback(self, callback)
        self.transport.send(cmd, callback=callback)

    @property
    def triggered(self):
        if not self.transport.io.inWaiting():
            return
        to = self.transport.timeout
        self.transport.timeout = .1
        line = self.transport.readline()
        self.transport.timeout = to
        return "TRG" in line

    def arm(self, dryrun=False):
        if dryrun:
            self.send("a")
        else:
            self.send("A")

    def disarm(self):
        self.send("D")

    def laser_toggle(self):
        self.send("L")

    def set_flash_delay(self, value):
        self.send("%dQ" % value)

def build_controller(port):
    tp = ControllerTransport(port, timeout=5)
    ctl = Controller(tp)
    return ctl
