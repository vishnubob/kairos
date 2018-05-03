#!/usr/bin/env python

from scad import *
nozzle = .4

class Bracket(SCAD_Object):
    anchor_width = inch2mm(1)
    anchor_dia = inch2mm(.25) + nozzle
    pump_dia = 3 + nozzle
    pump_screw_offset = 7.5
    pump_width = pump_screw_offset * 2
    height = 4.8
    #width = inch2mm(2)
    width = anchor_width + pump_width
    depth = inch2mm(1.75)

    def scad(self):
        plate = Cube(z=self.height, x=self.width, y=self.depth, center=True)
        plate = Translate(x=(self.width / 2.0) - self.anchor_width)(plate)
        anchor_screw_hole = Cylinder(d=self.anchor_dia, h=self.height + 1, center=True)
        anchor_screw_hole = Translate(x=self.anchor_width / -2.0)(anchor_screw_hole)

        pump_screw_hole = Cylinder(d=self.pump_dia, h=self.height + 1, center=True)
        pump_screw_hole = Translate(x=self.pump_screw_offset)(pump_screw_hole)
        countersink = Cylinder(d1=self.pump_dia, d2=self.pump_dia * 2, h=2, center=True)
        countersink = Translate(x=self.pump_screw_offset, z=(self.height / 2.0) - 1)(countersink)
        pump_screw_hole = Union()(pump_screw_hole, countersink)
        screws = Union()(anchor_screw_hole, pump_screw_hole)
        plate = Difference()(plate, screws)
        return plate

b = Bracket()
fn = "bracket"
b = SCAD_Globals(fn=50)(b)
b.render(fn + ".scad")
#b.render(fn + ".scad")
