$fn = 50;
 union() {
     difference() {
         translate([0.0, 0.0, 6.35]) {
             cylinder(r=6.2865, h=12.7, center=true);
        }

         union() {
             translate([0.0, 0.0, 2.6999999999999993]) {
                 union() {
                     translate([0.0, 0.0, 5.0]) {
                         cylinder(r=3.25, h=10.0, center=true);
                    }

                     translate([0.0, 0.0, 3.95]) {
                         union() {
                             cylinder(r=4.75, h=3.0, center=true);
                             translate([0.0, 0.0, -1.5]) {
                                 render() {
                                     difference() {
                                         cylinder(r1=3.25, r2=4.75, h=1.5, center=true);
                                         cylinder(r1=1.75, r2=3.4125, h=1.8, center=true);
                                    }

                                }

                            }

                             translate([0.0, 0.0, 1.5]) {
                                 render() {
                                     difference() {
                                         cylinder(r1=4.75, r2=3.25, h=1.5, center=true);
                                         cylinder(r1=3.4125, r2=1.75, h=1.8, center=true);
                                    }

                                }

                            }

                        }

                    }

                }

            }

             translate([0.0, 0.0, 1.3499999999999996]) {
                 cylinder(r1=5.2865, r2=3.25, h=2.8, center=true);
            }

             translate([0.0, 0.0, 12.7]) {
                 translate([0.0, 0.0, 0.75]) {
                     difference() {
                         render() {
                             difference() {
                                 cylinder(r=6.2865, h=1.5, center=true);
                                 cylinder(r=0.005, h=1.5, center=true);
                            }

                        }

                         translate([0.0, 0.0, -0.375]) {
                             cylinder(r1=1.5, r2=0.005, h=0.75, center=true);
                        }

                    }

                }

            }

        }

    }

     translate([0.0, 0.0, 12.7]) {
         translate([0.0, 0.0, 0.75]) {
             difference() {
                 render() {
                     difference() {
                         cylinder(r=6.2865, h=1.5, center=true);
                         cylinder(r=0.005, h=1.5, center=true);
                    }

                }

                 translate([0.0, 0.0, -0.375]) {
                     cylinder(r1=1.5, r2=0.005, h=0.75, center=true);
                }

            }

        }

    }

}
