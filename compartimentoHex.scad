//radio exterior
rE = 17*2.54/2;
//espesor da base
eB = 2;
//radio do recorte dos furados
rR = 6;
//altura do depósito
hTotal=30;

//translate([-2.54*9.5,0,0]) rotate ([180,0,0])
base();
//rotate([0,0,300]) caixa();
//translate([rE*2.5,0,0]) tapa();
module base(){
//base
difference(){
//base e borde
translate([2.54*9.5,0,0]) cylinder(r=23*2.54/2,h=3,$fn=6);
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
//e os furados para enganchar a caixa
    translate([2.54*9.5,0,0]) rotate([0,0,60]) furados();
//furados para pasar o fío, van un de máis porque non sabemos por onde vai quedar
    union(){
    translate([2.54*2,0,0]) cylinder(r=0.6, h=eB*2,$fn=10);
    translate([2.54*19,0,0]) cylinder(r=0.6, h=eB*2,$fn=10);
     translate([2.54*4,2.54*7,0]) cylinder(r=0.6, h=eB*2,$fn=10);
    translate([2.54*14,2.54*7,0]) cylinder(r=0.6, h=eB*2,$fn=10);
     translate([2.54*4,-2.54*7,0]) cylinder(r=0.6, h=eB*2,$fn=10);
    translate([2.54*14,-2.54*7,0]) cylinder(r=0.6, h=eB*2,$fn=10);
    }

}
}

module tapa(){
cylinder(r=rE,h=eB,$fn=6);
cylinder(r=rE-2.84/2,h=eB*2,$fn=6); //reborde da tapa máis pequeno que o oco para que encaixe
//saínte para enganchar o fío de suxección
difference(){
translate([rE-2,-4,0]) rotate([0,0,30]) cylinder(r=2,h=eB,$fn=6);
translate([rE-2,-4,-1]) rotate([0,0,30]) cylinder(r=1,h=eB+4,$fn=6);
}
}


module caixa(){
difference(){
//base e borde
union(){
//exterior
cylinder(r=rE,h=hTotal,$fn=6);
//asas
translate([8,-5,hTotal/5])rotate([0,15,0])  cube([10,10,25]);
translate([-18,-5,hTotal/5-2.5])  rotate([0,345,0])cube([10,10,25]);
    }
//hexagono interior
translate([0,0,2])cylinder(r=rE-2.54/2,h=hTotal+10,$fn=6);
//furados das orellas
translate([rE+0.8,0,0]) cylinder(r=0.6,h=hTotal+10,$fn=6);
translate([rE+0.8,-3,0]) cylinder(r=0.6,h=hTotal+10,$fn=6);
translate([-rE-0.8,0,0]) cylinder(r=0.6,h=hTotal+10,$fn=6);

furados();
}
}
module furados(){
translate([-3.5*2.54,3*2.54,-1]) cylinder (r=1.5,h=5,$fn=20);
translate([3.5*2.54,0,-1]) cylinder (r=1.5,h=5,$fn=20);
translate([-3.5*2.54,-3*2.54,-1]) cylinder (r=1.5,h=5,$fn=20);
}

