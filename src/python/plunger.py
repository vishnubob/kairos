import time
from . transport import SerialTransport

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

    def __iter__(self):
        return iter(self.plungers)

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

class PlungerTransport(SerialTransport):
    DefaultBaudrate = 9600

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

class PlungerBase(object):
    STATUS_PUMP_ERROR = 0x0F
    STATUS_PUMP_READY = 0x20

    def __init__(self, addr=0, transport=None):
        self.addr = addr
        self.transport = transport
        self.commands = []
        self.cache = {}
        self._status = None

    def send(self, cmd, callback=None, key=None, wait=True, query=False, timeout=None):
        while 1:
            self.send_actual(cmd, callback=callback, key=key, wait=wait, query=query, timeout=timeout)
            if not self.cache["ready"] and wait:
                try:
                    self.wait(timeout=timeout)
                except TimeoutError:
                    self.transport.io.close()
                    self.transport.io.open()
                    continue
            break

    def send_actual(self, cmd, callback=None, key=None, wait=True, query=False, timeout=None):
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
        callback = wrap_callback(self, callback, key)
        cmd = self.command_block(cmd)
        if query:
            cmd += '\r\n'
        else:
            cmd += 'R\r\n'
        self.transport.send(cmd, callback=callback)

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

    def wait(self, timeout=None):
        ts = time.time()
        while not self.ready:
            time.sleep(1)
            if timeout and (time.time() - ts > timeout):
                raise TimeoutError(timeout)

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
        self.send('Q', wait=False, query=True)
        return self.cache["ready"]

    @property
    def status(self):
        self.send('Q', wait=False, query=True)
        return self.cache["status"]

    def get_speed(self):
        return self.cache.get("speed", 11)
    def set_speed(self, value):
        self.cache["speed"] = value
        cmd = "S%d" % value
        self.send(cmd)
    speed = property(get_speed, set_speed)

    def aspirate(self, value, **kw):
        cmd = "P%d" % value
        self.send(cmd)

    def dispense(self, value, **kw):
        cmd = "D%d" % value
        self.send(cmd, **kw)

    def move(self, value, **kw):
        cmd = "A%d" % value
        self.send(cmd, **kw)

    def stop(self):
        self.send('T')

    def get_position(self):
        self.send('?4', key="position", wait=False, callback=int)
        return self.cache["position"]
    def set_position(self, value):
        self.move(value)
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
            self.send('E')
        else:
            raise ValueError(value)
    valve = property(get_valve, set_valve)

class Plunger(PlungerBase):
    def fill(self, position=3000):
        self.valve = "input"
        self.aspirate(position)

    def flush(self, position=3000):
        self.valve = "output"
        self.dispense(position)

    def cycle(self, fill_position=3000, fill_speed=1, flush_position=3000, flush_speed=20):
        self.speed = fill_speed
        self.fill(position=fill_position, speed=fill_speed)
        self.speed = flush_speed
        self.flush(position=flush_position, speed=flush_speed)

def build_manager(port):
    tp = PlungerTransport(port)
    pm = PlungerManager(tp)
    return pm
