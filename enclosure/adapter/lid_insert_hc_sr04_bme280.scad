// Lid insert remix for HC-SR04 + BME280 on a ~70mm canning-lid style disk.
// Print as a replacement for sourd.io insert_v25 when using common junk-box sensors.
// Units: millimeters
//
// Render:
//   openscad -o lid_insert_hc_sr04_bme280.stl -D 'PART="lid_insert"' lid_insert_hc_sr04_bme280.scad
//   openscad -o board_adapter_nrf52840.stl -D 'PART="board_adapter"' lid_insert_hc_sr04_bme280.scad
// Or: ../scripts/render_enclosure.sh

$fn = 64;

PART = "lid_insert"; // "lid_insert" | "board_adapter"

lid_od = 70;
lid_id = 62;
thickness = 3;

// HC-SR04 transducer centers ~25mm apart; module ~45 x 20
us_hole_d = 16.5;
us_spacing = 25;
us_offset_y = 8;

// BME280 breakout clearance / vent
bme_w = 16;
bme_h = 12;
bme_offset_y = -18;

// Board bay under upper stack — nRF52840 Feather / XIAO pocket
module board_adapter() {
  difference() {
    cube([55, 30, 4], center = true);
    // Feather-ish outline pocket
    translate([0, 0, 1]) cube([51, 23, 4], center = true);
    // screw holes M2.2-ish
    for (x = [-22, 22], y = [-10, 10])
      translate([x, y, 0]) cylinder(h = 10, d = 2.4, center = true);
  }
}

module lid_insert() {
  difference() {
    cylinder(h = thickness, d = lid_od);
    translate([0, 0, -1]) cylinder(h = thickness + 2, d = lid_id - 4);

    // Ultrasonic transducer holes
    translate([-us_spacing / 2, us_offset_y, -1])
      cylinder(h = thickness + 2, d = us_hole_d);
    translate([us_spacing / 2, us_offset_y, -1])
      cylinder(h = thickness + 2, d = us_hole_d);

    // BME280 vent / recess
    translate([0, bme_offset_y, thickness / 2])
      cube([bme_w, bme_h, thickness + 2], center = true);
  }
}

if (PART == "board_adapter") {
  board_adapter();
} else {
  lid_insert();
}
