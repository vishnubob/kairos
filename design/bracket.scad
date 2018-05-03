$fn = 50;
 difference() {
     translate([-5.199999999999999, 0.0, 0.0]) {
         cube([40.400000000000006, 44.45, 4.8], center=true);
    }

     union() {
         translate([-12.700000000000001, 0.0, 0.0]) {
             cylinder(r=3.375, h=5.8, center=true);
        }

         union() {
             translate([7.5, 0.0, 0.0]) {
                 cylinder(r=1.7, h=5.8, center=true);
            }

             translate([7.5, 0.0, 1.4]) {
                 cylinder(r1=1.7, r2=3.4, h=2.0, center=true);
            }

        }

    }

}
