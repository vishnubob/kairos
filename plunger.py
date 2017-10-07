import serial

VALVE_NONE = 0
VALVE_LEFT = 1
VALVE_RIGHT = 2

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
            if plunger.get_firmware():
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
        block = command + '\r\n'
        block = block.encode(self.encoding)
        self.io.write(block)
        resp = self.io.readline()
        resp = resp.decode(self.encoding)
        callback(resp)

class Plunger(object):
    class PlungerError(Exception):
        pass

    def __init__(self, addr=0, transport=None):
        self.addr = addr
        self.transport = transport
        self.commands = []
        self.cache = {}
        self._status = None

    @property
    def get_status(self):
        return self._status

    def send(self, cmd, callback=None):
        def wrap_callback(plunger, callback):
            def wrapper(resp):
                if not resp:
                    value = ''
                    status = None
                else:
                    (status, value) = self.answer_block(resp)
                plunger._status = status
                if callback:
                    callback(value)
            return wrapper
        cmd = self.command_block(cmd)
        callback = wrap_callback(self, callback)
        return self.transport.send(cmd, callback=callback)

    def get_firmware(self):
        if "firmware" not in self.cache:
            self.send('&', callback=self.set_firmware)
        return self.cache['_firmware']
    def set_firmware(self, value):
        self.cache["_firmware"] = value
    firmware = property(get_firmware, set_firmware)

    def answer_block(self, response):
        if response[:2] != '/0':
            raise PlungerError('parse')
        if response[-3:] != '\x03\r\n':
            raise PlungerError('parse')
        status = response[2]
        datablock = response[3:-3]
        return (status, datablock)

    def command_block(self, data_block):
        command_block = '/%X%s' 
        command_block %= (self.addr, data_block)
        return command_block

    def initialize(self, full_force=True, valve=VALVE_RIGHT):
        command_map = {
            VALVE_NONE: 'W%d',
            VALVE_RIGHT: 'Z%d',
            VALVE_LEFT: 'Y%d',
        }
        n = 0 if full_force else 1
        cstr = command_map[valve] % n
        return cstr
    
    def get_valve(self):
        return self._valve
    def set_valve(self, value):
        if value == VALVE_INPUT:
            self.emit('I')
            self._valve = VAVLE_INPUT
        elif value == VALVE_OUTPUT:
            self.emit('O')
            self._valve = VAVLE_OUTPUT
        elif value == VALVE_BYPASS:
            self.emit('B')
            self._valve = VAVLE_OUTPUT
        elif value == VALVE_EXTRA:
            self.emit('B')
            self._valve = VAVLE_OUTPUT
        else:
            raise ValueError(value)
    valve = property(get_valve, set_valve)


port = "/dev/ttyUSB0"
tp = PlungerTransport(port)
pm = PlungerManager(tp)
print(pm.plungers)
print (pm[0].firmware)
print (pm[0].status)
