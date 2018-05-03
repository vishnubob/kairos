#!/usr/bin/env python

from scad import *
nozzle = .4

class LaserCozy(SCAD_Object):
    laser_dia = 6 + .5
    laser_height = 10
    laser_head_height = 5.55
    #laser_tail_height = 3.5
    #ring_height = laser_height - (laser_head_height + laser_tail_height)
    ring_height = 2
    laser_tail_height = laser_height - ring_height - laser_head_height
    ring_height = 3
    ring_dia = 9.0 + .5

    holder_dia = inch2mm(.5) * 0.99
    holder_height = inch2mm(.5)
    cover_height = 1.5
    aperture_dia = .01
    aperture_laser_dia = 3
    chamfer = ring_height - .5

    @property
    def laser(self):
        laser = Cylinder(d=self.laser_dia, h=self.laser_height, center=True)
        laser = Translate(z=self.laser_height / 2.0)(laser)
        #ch = 1
        #chamfer0 = Chamfer(radius=(self.laser_dia - ch) / 2.0, chamfer=ch, invert=True, center=True)
        #chamfer0 = Translate(z=(ch / 2.0))(chamfer0)
        #laser = Union()(laser, chamfer0)
        # ring
        #ring = Pipe(iD=self.ring_dia, oD=self.laser_dia + 2, h=self.ring_height, center=True)
        ring = Cylinder(dia=self.ring_dia, h=self.ring_height, center=True)

        chamfer1 = Chamfer(radius=self.laser_dia / 2.0, chamfer=self.ring_height / 2.0, invert=False, center=True)
        chamfer1 = Translate(z=self.ring_height / -2.0)(chamfer1)

        chamfer2 = Chamfer(r=self.laser_dia / 2.0, chamfer=self.ring_height / 2.0, invert=True, center=True)
        chamfer2 = Translate(z=self.ring_height / 2.0)(chamfer2)

        ring = Union()(ring, chamfer1, chamfer2)
        ring = Translate(z=(self.ring_height / 2.0) + self.laser_tail_height)(ring)
        laser = Union()(laser, ring)
        return laser

    @property
    def aperture(self):
        cover = Pipe(oD=self.holder_dia, iD=self.aperture_dia, h=self.cover_height, center=True)
        cone = Cylinder(d2=self.aperture_dia, d1=self.aperture_laser_dia, h=self.cover_height / 2.0, center=True)
        cone = Translate(z=self.cover_height / -4.0)(cone)
        cover = Difference()(cover, cone)
        cover = Translate(z=self.cover_height / 2.0)(cover)
        return cover

    def scad(self):
        offset = self.holder_height - self.laser_height
        thickness = self.holder_dia - 2
        # shell
        holder = Cylinder(dia=self.holder_dia, h=self.holder_height, center=True)
        holder = Translate(z=self.holder_height / 2.0)(holder)
        # cavity
        laser = Translate(z=offset)(self.laser)
        # .1 = overcut
        cavity = Cylinder(d1=thickness, d2=self.laser_dia, h=offset + .1, center=True)
        cavity = Translate(z=offset / 2.0)(cavity)
        aperture = Translate(z=self.laser_height + offset)(self.aperture)
        cavity = Union()(laser, cavity, aperture)
        # final product
        holder = Difference()(holder, cavity)
        holder = Union()(holder, aperture)
        return holder

b = LaserCozy()
#b = b.laser
#b = b.aperture
fn = "laser_cozy.scad"
b = SCAD_Globals(fn=50)(b)
b.render(fn + ".scad")
