#!/usr/bin/env python
from timestop import director

d = director.Director()
pump = d.pumps[0]
while 1:
    print(d.drip_once(0))
