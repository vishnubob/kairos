EarthGravity = 9.81

class TimerCalculator(object):
    DefaultFrequency = 16e6
    Prescaler = [1, 8, 32, 32, 64, 128, 256, 1024]

    def __init__(self, frequency=DefaultFrequency, prescaler=1):
        self.frequency = frequency
        self.prescaler = 1

    def real_time(self, tick, prescaler=None):
        prescaler = prescaler if prescaler != None else self.prescaler
        return tick / float(self.frequency / prescaler)

    def real_time_all(self, tick):
        return {ps: self.real_time(tick, prescaler=ps) for ps in self.PrescalerValues}

    def ticks(self, real_time, prescaler=None):
        prescaler = prescaler if prescaler != None else self.prescaler
        ticks = real_time * float(self.frequency / prescaler)
        int_ticks = int(round(ticks))
        real_time_int = self.real_time(int_ticks, prescaler=prescaler)
        return (int_ticks, abs(real_time - real_time_int))

    def ticks_all(self, real_time):
        return {ps: self.ticks(real_time, prescaler=ps) for ps in self.PrescalerValues}

class FallingBody(object):
    def __init__(self, **kw):
        keys = map(str.upper, kw.keys())
        self.gravity = kw.get('g', EarthGravity)
        self._distance = kw.get('d', None)
        self._time = kw.get('t', None)
        assert self._distance != None or self._time != None
        self.g = self.G = self.gravity

    @property
    def time(self):
        if self._time != None:
            return self._time
        return ((2 * self._distance) / self.gravity) ** .5
    t = T = time

    @property
    def distance(self):
        if self._distance != None:
            return self._distance
        return (.5 * self.gravity) * (self._time ** 2)
    d = D = distance

    @property
    def velocity_instantaneous(self):
        return self.gravity * self.time
    vi = Vi = velocity_instantaneous

    @property
    def velocity_average(self):
        return .5 * (g * t)
    va = Va = velocity_average
fall = FallingBody

def test():
    val = 5
    assert fall(t=val).d == (.5 * (EarthGravity * val ** 2))
    assert fall(d=val).t == ((2.0 * val) / EarthGravity) ** .5

if __name__ == "__main__":
    test()
