import serial
import time

VALVE_NONE = None
VALVE_LEFT = "left"
VALVE_RIGHT = "right"

VALVE_INPUT = "input"
VALVE_OUTPUT = "output"
VALVE_BYPASS = "bypass"
VALVE_EXTRA = "extra"

class PlungerManager(object):
    MaxPlungers = 16

    def __init__(self, transport):
        self.transport = transport
        self.plungers = self.scan()

    def __getitem__(self, index):
        return self.plungers[index]

    def scan(self):
        self.transport.timeout = .05
        plungers = []
        for addr in range(self.MaxPlungers):
            plunger = Plunger(addr, self.transport)
            if plunger.firmware:
                plungers.append(plunger)
        self.transport.timeout = 1
        return plungers

class PlungerTransport(object):
    Encoding = "latin-1"

    def __init__(self, port, baud=9600, timeout=1, encoding=Encoding):
        self.port = port
        self.io = serial.Serial(port, baudrate=baud, timeout=timeout)
        self.encoding = encoding

    def get_timeout(self):
        return self.io.timeout
    def set_timeout(self, value):
        self.io.timeout = value
    timeout = property(get_timeout, set_timeout)

    def send(self, command, callback):
        block = command + 'R\r\n'
        block = block.encode(self.encoding)
        self.io.write(block)
        resp = self.io.readline()
        resp = resp.decode(self.encoding)
        callback(resp)
        print("->", block.strip(), "<-", resp.strip())

class PlungerError(Exception):
    ErrorCodes = {
        0x00: "Error Free",
        0x01: "Initialization error",
        0x02: "Invalid command",
        0x03: "Invalid operand",
        0x04: "Invalid command sequence",
        0x05: "Fluid detection",
        0x06: "EEPROM failure",
        0x07: "Device not initialized",
        0x09: "Plunger overload",
        0x10: "Valve overload",
        0x11: "Plunger move not allowed",
        0x15: "Command overflow",
    }

    def __init__(self, error_code):
        self.error_code = error_code

    def __str__(self):
        return self.ErrorCodes[self.error_code]

class Plunger(object):
    STATUS_PUMP_ERROR = 0x0F
    STATUS_PUMP_READY = 0x20

    def __init__(self, addr=0, transport=None):
        self.addr = addr
        self.transport = transport
        self.commands = []
        self.cache = {}
        self._status = None

    def send(self, cmd, callback=None, key=None, wait=True):
        def wrap_callback(plunger, callback, key):
            def wrapper(resp):
                if not resp:
                    value = ''
                    status = None
                    ready = False
                    error = 0
                else:
                    (status, value) = self.answer_block(resp)
                    status = ord(status)
                    ready = bool(status & self.STATUS_PUMP_READY)
                    error = (status & self.STATUS_PUMP_ERROR)
                self.cache["status"] = status
                self.cache["ready"] = ready
                self.cache["error"] = error
                if callback:
                    value = callback(value)
                if key:
                    self.cache[key] = value
                if error:
                    raise PlungerError(error)
            return wrapper
        cmd = self.command_block(cmd)
        callback = wrap_callback(self, callback, key)
        self.transport.send(cmd, callback=callback)
        if wait:
            self.wait()

    def answer_block(self, response):
        slash = response.index('/0')
        if slash == '-1':
            return (None, None)
        terminal = response.index('\x03\r\n')
        if terminal == '-1':
            return (None, None)
        response = response[slash+2:terminal]
        status = response[0]
        datablock = response[1:]
        return (status, datablock)
    
    def command_block(self, data_block):
        command_block = '/%X%s' 
        command_block %= (self.addr, data_block)
        return command_block

    def wait(self):
        while not self.ready:
            time.sleep(1)

    def initialize(self, full_force=False, input_valve=VALVE_RIGHT):
        command_map = {
            VALVE_NONE: 'W%d',
            VALVE_RIGHT: 'Z%d',
            VALVE_LEFT: 'Y%d',
        }
        n = 0 if full_force else 1
        cstr = command_map[input_valve] % n
        self.send(cstr)
    
    @property
    def firmware(self):
        if "firmware" not in self.cache:
            self.send('&', key="firmware", wait=False)
        return self.cache['firmware']

    @property
    def ready(self):
        self.send('Q', wait=False)
        return self.cache["ready"]

    @property
    def status(self):
        self.send('Q', wait=False)
        return self.cache["status"]

    def get_speed(self):
        return self.cache.get("speed", 11)
    def set_speed(self, value):
        self.cache["speed"] = value
        cmd = "S%d" % value
        self.send(cmd)
    speed = property(get_speed, set_speed)

    def get_position(self):
        self.send('?4', key="position", wait=False, callback=int)
        return self.cache["position"]
    def set_position(self, value):
        cmd = "A%d" % value
        self.send(cmd)
    position = property(get_position, set_position)

    def get_valve(self):
        return self._valve
    def set_valve(self, port):
        if port == VALVE_INPUT:
            self.send('I')
        elif port == VALVE_OUTPUT:
            self.send('O')
        elif port == VALVE_BYPASS:
            self.send('B')
        elif port == VALVE_EXTRA:
            self.send('B')
        else:
            raise ValueError(value)
    valve = property(get_valve, set_valve)


def cycle(plunger):
    plunger.speed = 1
    plunger.valve = "input"
    plunger.position = 3000
    plunger.speed = 20
    plunger.valve = "output"
    plunger.position = 0

port = "./pumps"
tp = PlungerTransport(port)
pm = PlungerManager(tp)
plunger = pm.plungers[0]
plunger.initialize()
speed = 12
plunger.speed = 4
while 1:
    #plunger.speed = speed
    #speed -= 1
    cycle(plunger)
