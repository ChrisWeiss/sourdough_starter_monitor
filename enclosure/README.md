# Enclosure

## Vendored sourd.io v2.5

STL files in [`v2.5/`](v2.5/) are from [twilio/sourd.io](https://github.com/twilio/sourd.io) (Apache-2.0). Designed for a Ball jar with a **2½″ canning lid** (removable center).

| File | Role |
|------|------|
| `insert_v25.stl` | Original Grove sensor insert |
| `layer1_v25.stl` … `layer4_v25.stl` | Stack / board bay for Narrowband kit |
| `hat-v25.stl` | Decorative hat |

## Remix for this project

This project uses **VL53L3CX** (optical window) + **BME280**.

OpenSCAD source: [`adapter/lid_insert_vl53l3cx_bme280.scad`](adapter/lid_insert_vl53l3cx_bme280.scad)

| Output | Role |
|--------|------|
| [`adapter/lid_insert_vl53l3cx_bme280.stl`](adapter/lid_insert_vl53l3cx_bme280.stl) | ToF aperture + BME280 vent on a ~70 mm disk |
| [`adapter/board_adapter_nrf52840.stl`](adapter/board_adapter_nrf52840.stl) | Shallow pocket for Feather / XIAO-sized nRF52840 |

Render both STLs (or re-tune after measuring your modules):

```bash
./scripts/render_enclosure.sh
```

In the OpenSCAD GUI, use **Customizer → Part** (or edit `PART` at the top of the `.scad`) to switch between `lid_insert` and `board_adapter` for preview. CLI/`render_enclosure.sh` pass `-D PART=…` so both STLs still export without editing the file.

Tune `tof_window_d` and board pocket dimensions in the `.scad` before printing. Keep the dough surface **≥ 10 mm** from the sensor (prefer ≥ 30 mm). Fit-check against a Ball jar lid and the vendored v2.5 stack before committing to a long print.
