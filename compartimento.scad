//radio interior
ri = 40;
//espesor
e = 3;
//altura
a=30;
//caixa
module caixa(){
    difference(){
        union(){
        cylinder (r = ri+e, h= a, $fn=60);
            translate ([ri-10,0,a/2]) difference(){
rotate ([0,45,0]) cube ([10,10,20]);
translate([15,13,8]) rotate ([90,0,0]) cylinder(r=1, h=16, $fn=20);
}
translate ([-ri,0,a/4]) difference(){
rotate ([0,-45,0]) cube ([10,10,20]);
translate([-8,13,14]) rotate ([90,0,0]) cylinder(r=1, h=16, $fn=20);
}

}
        translate([0,0,e]) cylinder (r = ri, h= a, $fn=60);}
}

module tapa() {
cylinder(r=ri+e,h=2,$fn=80);
cylinder(r=ri-0.1,h=4,$fn=80);
}

caixa();
translate([90,0,0]) tapa();