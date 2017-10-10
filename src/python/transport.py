import time
import serial

__all__ = ["Transport", "SerialTransport"]

class Transport(object):
    pass

class SerialTransport(Transport):
    Encoding = "ascii"
    DefaultBaudrate = 9600

    def __init__(self, port, baudrate=None, timeout=1, encoding=Encoding):
        self.port = port
        baudrate = baudrate if baudrate != None else self.DefaultBaudrate
        self.io = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        self.encoding = encoding

    def readline(self):
        resp = self.io.readline()
        resp = resp.decode(self.encoding, errors="ignore")
        return resp

    def send(self, command, callback):
        command = command.encode(self.encoding)
        self.io.write(command)
        self.io.flush()
        resp = self.readline()
        self.log("->", repr(command), "<-", repr(resp))
        callback(resp)

    def reset(self):
        self.io.setDTR(1)
        time.sleep(.1)
        self.io.setDTR(0)

    def log(self, *args):
        msg = str.join(' ', map(str, args))
        msg = "%s: %s" % (self.__class__.__name__, msg)
        #print(msg)

    def get_timeout(self):
        return self.io.timeout
    def set_timeout(self, value):
        self.io.timeout = value
    timeout = property(get_timeout, set_timeout)
