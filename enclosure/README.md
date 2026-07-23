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

| Rendered STL | Role |
|--------------|------|
| [`adapter/lid_insert_hc_sr04_bme280.stl`](adapter/lid_insert_hc_sr04_bme280.stl) | Dual transducer holes + BME280 vent on a ~70 mm disk |
| [`adapter/board_adapter_nrf52840.stl`](adapter/board_adapter_nrf52840.stl) | Shallow pocket for Feather / XIAO-sized nRF52840 |

Render (or re-tune after measuring your modules):

```bash
./scripts/render_enclosure.sh
```

Tune `us_hole_d`, `us_spacing`, and board pocket dimensions in the `.scad` before printing. Fit-check against a Ball jar lid and the vendored v2.5 stack before committing to a long print.
