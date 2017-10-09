EarthGravity = 9.81

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
