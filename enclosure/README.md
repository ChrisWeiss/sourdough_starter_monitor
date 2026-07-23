# Enclosure

## Vendored sourd.io v2.5

STL files in [`v2.5/`](v2.5/) are from [twilio/sourd.io](https://github.com/twilio/sourd.io) (Apache-2.0). Designed for a Ball jar with a **2½″ canning lid** (removable center).

| File | Role |
|------|------|
| `insert_v25.stl` | Original Grove sensor insert |
| `layer1_v25.stl` … `layer4_v25.stl` | Stack / board bay for Narrowband kit |
| `hat-v25.stl` | Decorative hat |

## Remix for this project

Original insert cutouts are Grove-shaped. This project uses **HC-SR04** + **BME280**.

OpenSCAD source: [`adapter/lid_insert_hc_sr04_bme280.scad`](adapter/lid_insert_hc_sr04_bme280.scad)

- `lid_insert()` — dual transducer holes + BME280 vent on a ~70 mm disk
- `board_adapter()` — shallow pocket for Feather / XIAO-sized nRF52840 in the upper bay

Render STLs with [OpenSCAD](https://openscad.org/):

```bash
openscad -o adapter/lid_insert_hc_sr04_bme280.stl adapter/lid_insert_hc_sr04_bme280.scad
```

Tune `us_hole_d`, `us_spacing`, and board pocket dimensions to your exact modules before printing.
