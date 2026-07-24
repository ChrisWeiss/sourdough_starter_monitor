// Lid insert remix for VL53L3CX + BME280 on a ~70mm canning-lid style disk.
// Units: millimeters
//
// Which part to render (GUI Customizer dropdown, or edit PART below):
//   lid_insert     — ToF optical window + BME280 vent (~70 mm disk)
//   board_adapter  — Feather / XIAO board pocket
//
// CLI:
//   openscad -o lid_insert_vl53l3cx_bme280.stl -D 'PART="lid_insert"' lid_insert_vl53l3cx_bme280.scad
//   openscad -o board_adapter_nrf52840.stl -D 'PART="board_adapter"' lid_insert_vl53l3cx_bme280.scad
// Or: ./scripts/render_enclosure.sh

$fn = 64;

/* [Part] */
PART = "lid_insert"; // [lid_insert, board_adapter]

lid_od = 70;
lid_id = 62;
thickness = 3;

// VL53L3CX aperture — keep dough farther than 10 mm (prefer ≥ 30 mm)
tof_window_d = 8;
tof_offset_y = 8;

// BME280 breakout clearance / vent
bme_w = 16;
bme_h = 12;
bme_offset_y = -18;

module board_adapter() {
  difference() {
    cube([55, 30, 4], center = true);
    translate([0, 0, 1]) cube([51, 23, 4], center = true);
    for (x = [-22, 22], y = [-10, 10])
      translate([x, y, 0]) cylinder(h = 10, d = 2.4, center = true);
  }
}

module lid_insert() {
  difference() {
    cylinder(h = thickness, d = lid_od);
    translate([0, 0, -1]) cylinder(h = thickness + 2, d = lid_id - 4);

    // ToF optical window (centered on sensor FOV)
    translate([0, tof_offset_y, -1])
      cylinder(h = thickness + 2, d = tof_window_d);

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
