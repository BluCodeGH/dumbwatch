function aang(length, r) = 360*length / 2 / PI / r;
module wrist() {
  length = 50;
  max_width = 49;
  max_height = 32;
  bone1_r = 25/2;
  bone2_r = 28/2;
  tendon_r = 10/2;
  tendon_to_bone1_edge = 25;
  color("#ffe9d1")
  rotate([90, 0, 0])
  hull() {
    translate([-(max_width/2 - bone1_r), -bone1_r, 0])
      cylinder(h=length, r=bone1_r);
    translate([max_width/2 - bone2_r, -bone2_r, 0])
      cylinder(h=length, r=bone2_r);
    translate([-(max_width/2 - tendon_to_bone1_edge), -(max_height - tendon_r), 0])
      cylinder(h=length, r=tendon_r);
  }
}

disp_len = 59.2;
disp_width = 29.2;
disp_thickness = 1;
flex_width = 15.5 - 1.4;
flex_len = 14.3;
module display() {
  cube([disp_len, 29.2, 0.3]);
  mirror([1, 0, 0]) {
    translate([0, 8.56, 0])
      cube([14.3, 12.5, 0.3]);
    translate([0, 6.75, 0])
      cube([14.3 - 10.3, 15.5 - 1.4, 0.3]);
  }
}

module curve_chain(r, length, width) {
  // 2*pi*r * theta/360 = length
  arc_angle = 360 * length / 2 / PI / r;
  translate([width/2, 0, -r])
  rotate([90,-90,-90])
  union() {
    rotate_extrude(angle=arc_angle, convexity=1)
    difference() {
      square([r, width]);
      square([r - disp_thickness, width]);
    }
    rotate([0, 0, arc_angle-90])
    translate([0, r, width/2])
    rotate([00, 90, 90])
    children();
  }
}

module bent_display(disp_r, flex_r, flex_l) {
  curve_chain(disp_r, disp_len, disp_width) {
  curve_chain(disp_r, (flex_len - flex_l) / 2, flex_width) {
  curve_chain(flex_r, flex_l, flex_width) {
  curve_chain(disp_r, (flex_len - flex_l) / 2, flex_width) {
    children();
  }}}}
}

/*
wrist();

translate([25, -25, -6])
rotate([47, 0, 90])
// 34mm comes from some awful math for a 52mm chord length
bent_display(34, 5, 5) {
  color("green")
  translate([0, -3, -2])
  import("pcb.stl", convexity=2);
}*/

wall = 2;
difference() {
  color("grey")
  //cylinder(r=34+wall+disp_thickness,h=disp_width + wall*2);
  rotate([0, 0, -aang(wall, 34 + disp_thickness)])
  rotate_extrude(angle=360*(disp_len + wall)/2/PI/(34 + disp_thickness), convexity=2)
  square([34 + wall + disp_thickness, disp_width + wall*2]);
  translate([0, 0, -wall])
  cylinder(r=34-wall,h=disp_width + wall*4);
  translate([0, 0, wall*2])
  difference() {
    cylinder(r=34+wall*5,h=disp_width - wall*2);
    translate([0, 0, -wall])
    cylinder(r=34,h=disp_width);
  }
  translate([34+disp_thickness, 0, disp_width/2 + wall])
  rotate([0, 90, 0])
  bent_display(34+disp_thickness, 5, 5) {};
}

