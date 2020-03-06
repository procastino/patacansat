//espesor da base
eB = 2;
//radio do recorte dos furados
rR = 6;
//placa para soster as baterías
difference(){
//base e borde
translate([2.54*9.5,0,0]) cylinder(r=23*2.54/2,h=5,$fn=6);
translate([2.54*9.5,0,eB]) cylinder(r=22*2.54/2,h=6,$fn=6);
//furados e recorte da beira
//furado so
cylinder(r=1.5, h=10, $fn=20);
translate([0,0,eB]) cylinder(r=rR, h=10, $fn=6);
//furado
translate([16*2.54,6*2.54,0]) cylinder(r=1.5, h=10, $fn=20);
translate([16*2.54,6*2.54,eB]) cylinder(r=rR, h=10, $fn=6);
//furado
translate([16*2.54,-6*2.54,eB]) cylinder(r=rR, h=10, $fn=6);
translate([16*2.54,-6*2.54,0]) cylinder(r=1.5, h=10, $fn=20);

}

translate([2.54*3+2,-5,0]) separadores();

module separadores(){
cube([1.54,2.54*5,6]);
translate([26+1.54,0,0]) cube([1.54,2.54*5,6]);
}
translate([6*2.54,8.5*2.54,0]) cube([2.54*6,2.9,5]);