#!/usr/bin/env python3
"""Minimal STL writer for the lid insert + board adapter when OpenSCAD is unavailable."""

from __future__ import annotations

import math
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "enclosure" / "adapter"


def write_binary_stl(path: Path, triangles: list[tuple[tuple[float, float, float], ...]]) -> None:
    # Each triangle: (n, v0, v1, v2) — n computed if needed
    buf = bytearray()
    buf.extend(b"sourdough enclosure"[0:80].ljust(80, b"\0"))
    buf.extend(struct.pack("<I", len(triangles)))
    for tri in triangles:
        v0, v1, v2 = tri
        ux, uy, uz = (v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2])
        vx, vy, vz = (v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2])
        nx = uy * vz - uz * vy
        ny = uz * vx - ux * vz
        nz = ux * vy - uy * vx
        norm = math.sqrt(nx * nx + ny * ny + nz * nz) or 1.0
        nx, ny, nz = nx / norm, ny / norm, nz / norm
        buf.extend(struct.pack("<3f", nx, ny, nz))
        for v in (v0, v1, v2):
            buf.extend(struct.pack("<3f", *v))
        buf.extend(struct.pack("<H", 0))
    path.write_bytes(buf)


def add_box(tris: list, x0, y0, z0, x1, y1, z1) -> None:
    # 12 triangles for an axis-aligned box (outward normals approximate)
    v = [
        (x0, y0, z0),
        (x1, y0, z0),
        (x1, y1, z0),
        (x0, y1, z0),
        (x0, y0, z1),
        (x1, y0, z1),
        (x1, y1, z1),
        (x0, y1, z1),
    ]
    faces = [
        (0, 1, 2, 3),  # bottom
        (4, 7, 6, 5),  # top
        (0, 4, 5, 1),  # front
        (1, 5, 6, 2),  # right
        (2, 6, 7, 3),  # back
        (3, 7, 4, 0),  # left
    ]
    for a, b, c, d in faces:
        tris.append((v[a], v[b], v[c]))
        tris.append((v[a], v[c], v[d]))


def cylinder_shell(tris, r_outer, r_inner, h, segments=48) -> None:
    """Annulus (disk with center hole) extruded to height h, centered on Z."""
    for i in range(segments):
        a0 = 2 * math.pi * i / segments
        a1 = 2 * math.pi * (i + 1) / segments
        xo0, yo0 = r_outer * math.cos(a0), r_outer * math.sin(a0)
        xo1, yo1 = r_outer * math.cos(a1), r_outer * math.sin(a1)
        xi0, yi0 = r_inner * math.cos(a0), r_inner * math.sin(a0)
        xi1, yi1 = r_inner * math.cos(a1), r_inner * math.sin(a1)
        # top ring
        tris.append(((xo0, yo0, h), (xo1, yo1, h), (xi1, yi1, h)))
        tris.append(((xo0, yo0, h), (xi1, yi1, h), (xi0, yi0, h)))
        # bottom ring
        tris.append(((xo0, yo0, 0), (xi0, yi0, 0), (xi1, yi1, 0)))
        tris.append(((xo0, yo0, 0), (xi1, yi1, 0), (xo1, yo1, 0)))
        # outer wall
        tris.append(((xo0, yo0, 0), (xo1, yo1, 0), (xo1, yo1, h)))
        tris.append(((xo0, yo0, 0), (xo1, yo1, h), (xo0, yo0, h)))
        # inner wall
        tris.append(((xi0, yi0, 0), (xi0, yi0, h), (xi1, yi1, h)))
        tris.append(((xi0, yi0, 0), (xi1, yi1, h), (xi1, yi1, 0)))


def hole_cut_approx(tris, cx, cy, r, h, segments=24) -> None:
    """Add a cylindrical void as an inverted shell (for visual STL preview / fit guides).

    True CSG subtraction needs OpenSCAD; this emits a separate hole guide cylinder
    slightly taller so slicers treating the file as a single body should prefer
    the OpenSCAD render. For the Python fallback we keep the annulus only and
    document transducer hole centers in the README.
    """
    del tris, cx, cy, r, h, segments


def lid_insert_tris() -> list:
    tris: list = []
    lid_od, lid_id, thickness = 70.0, 62.0, 3.0
    cylinder_shell(tris, lid_od / 2, (lid_id - 4) / 2, thickness)
    return tris


def board_adapter_tris() -> list:
    tris: list = []
    # Outer plate 55 x 30 x 4, pocket 51 x 23 x 3 from top
    add_box(tris, -27.5, -15, 0, 27.5, 15, 4)
    # Pocket as open top: leave a thinner floor by not filling the pocket solid —
    # approximate with a floor slab only under the pocket region
    add_box(tris, -25.5, -11.5, 0, 25.5, 11.5, 1)
    return tris


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    lid = OUT / "lid_insert_vl53l3cx_bme280.stl"
    board = OUT / "board_adapter_nrf52840.stl"
    write_binary_stl(lid, lid_insert_tris())
    write_binary_stl(board, board_adapter_tris())
    print(f"Wrote {lid} ({lid.stat().st_size} bytes)")
    print(f"Wrote {board} ({board.stat().st_size} bytes)")
    print(
        "Note: Python fallback omits CSG ToF/BME cutouts. "
        "Prefer OpenSCAD render via scripts/render_enclosure.sh when available."
    )


if __name__ == "__main__":
    main()
