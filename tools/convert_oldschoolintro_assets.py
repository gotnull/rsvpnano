#!/usr/bin/env python3
"""Convert Scrolls88 / oldschoolintro asset GIFs and embedded byte tables
into a C++ header.

Inputs:
  - tools/demoref/equinox/oldschoolintro/assets/fontes78.gif   (7x320, 1-bit)
  - tools/demoref/equinox/oldschoolintro/assets/fontes1517.gif (15x527, 1-bit)
  - The c[413] decoration mask packed into Scrolls88's constructor
  - The a[186] palette table packed into Scrolls88's constructor
  - The four scroller texts in a.java

Output:
  src/demos/oldschoolintro_assets.h
"""

from __future__ import annotations
import os
import re
import sys
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASSETS_DIR = os.path.join(ROOT, "tools/demoref/equinox/oldschoolintro/assets")
DECOMPILED_DIR = os.path.join(ROOT, "tools/demoref/equinox/oldschoolintro/decompiled")
HEADER_PATH = os.path.join(ROOT, "src/demos/oldschoolintro_assets.h")


def read_lit_mask(path: str):
    """Return a 1D list of 0/1 per pixel where 1 = LIT (= non-black in the GIF
    palette). Java's PixelGrabber returns ARGB; transparency is tested as
    `pixel == 0xFF000000`. We replicate that here by mapping each palette
    index to its RGB triplet."""
    img = Image.open(path)
    pal = img.getpalette() or []
    w, h = img.size
    raw = list(img.getdata())
    lit = []
    for v in raw:
        base = (v if isinstance(v, int) else 0) * 3
        if base + 2 >= len(pal):
            lit.append(0)
            continue
        r, g, b = pal[base], pal[base + 1], pal[base + 2]
        lit.append(0 if (r == 0 and g == 0 and b == 0) else 1)
    return lit, w, h


def parse_c_array(java_text: str) -> list[int]:
    """Extract the byArray[413] sparse assignments from Scrolls88()."""
    arr = [0] * 413
    pat = re.compile(r"byArray\[(\d+)\]\s*=\s*(-?\d+);")
    for m in pat.finditer(java_text):
        idx = int(m.group(1))
        val = int(m.group(2))
        if 0 <= idx < 413:
            # Java byte is signed; coerce to unsigned 0..255.
            arr[idx] = val & 0xFF
    return arr


def parse_a_array(java_text: str) -> list[int]:
    """Extract the nArray2[186] sparse assignments that feed `this.a`."""
    arr = [0] * 186
    pat = re.compile(r"nArray2\[(\d+)\]\s*=\s*(-?\d+);")
    for m in pat.finditer(java_text):
        idx = int(m.group(1))
        val = int(m.group(2))
        if 0 <= idx < 186:
            arr[idx] = val
    return arr


def parse_scroller_texts(a_java_text: str) -> dict[str, str]:
    texts = {}
    pat = re.compile(r'public String (b[stuv]) = "([^"]*)";')
    for m in pat.finditer(a_java_text):
        texts[m.group(1)] = m.group(2)
    return texts


def c_byte_table(name: str, data: bytes | list[int], width: int = 12) -> str:
    out = [f"constexpr uint8_t {name}[{len(data)}] = {{"]
    for i in range(0, len(data), width):
        chunk = data[i:i + width]
        out.append("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",")
    out.append("};")
    return "\n".join(out)


def c_int_table(name: str, data: list[int], width: int = 12) -> str:
    out = [f"constexpr int16_t {name}[{len(data)}] = {{"]
    for i in range(0, len(data), width):
        chunk = data[i:i + width]
        out.append("    " + ", ".join(str(v) for v in chunk) + ",")
    out.append("};")
    return "\n".join(out)


def c_string(name: str, s: str) -> str:
    # Escape backslash and quotes; keep printable ASCII verbatim.
    esc = s.replace("\\", "\\\\").replace('"', '\\"')
    return f'constexpr char {name}[] = "{esc}";'


def build_big_font(lit: list[int], w: int, h: int) -> list[int]:
    """Replicate Scrolls88.r(): reinterpret the GIF as 56x32, write
    1 + (i%56)/7 for LIT (non-black) pixels, else 0. Output 1792 bytes."""
    if w != 7 or h < 320:
        raise SystemExit(f"fontes78.gif unexpected size {w}x{h}")
    out = [0] * 1792
    for n in range(32):
        for n2 in range(56):
            src = n2 + n * 56
            if src >= len(lit):
                break
            if lit[src]:
                out[n2 + n * 56] = 1 + (n2 // 7)
    return out


def build_small_font(lit: list[int], w: int, h: int) -> list[int]:
    """Replicate Scrolls88.p(): reinterpret 15x527 as 255x31. ac[i]=1
    for LIT (non-black) pixels, else 0. Output 7905 bytes."""
    if w != 15 or h < 527:
        raise SystemExit(f"fontes1517.gif unexpected size {w}x{h}")
    out = [0] * 7905
    for n in range(31):
        for n2 in range(255):
            src = n2 + n * 255
            if src >= len(lit):
                break
            out[n2 + n * 255] = 1 if lit[src] else 0
    return out


def main() -> int:
    lit78, w78, h78 = read_lit_mask(os.path.join(ASSETS_DIR, "fontes78.gif"))
    lit15, w15, h15 = read_lit_mask(os.path.join(ASSETS_DIR, "fontes1517.gif"))

    big = build_big_font(lit78, w78, h78)
    small = build_small_font(lit15, w15, h15)

    with open(os.path.join(DECOMPILED_DIR, "Scrolls88.java")) as fh:
        java_text = fh.read()
    with open(os.path.join(DECOMPILED_DIR, "a.java")) as fh:
        a_java_text = fh.read()

    c_table = parse_c_array(java_text)
    a_table = parse_a_array(java_text)
    texts = parse_scroller_texts(a_java_text)

    sections = [
        "// Auto-generated by tools/convert_oldschoolintro_assets.py — do not edit.",
        "#pragma once",
        "",
        "#include <stdint.h>",
        "",
        "namespace oldschool {",
        "",
        "// fontes78.gif baked into Java's 56x32 \"o[]\" buffer. Values 1..8 mark",
        "// a non-empty pixel and select the per-row colour band; 0 = transparent.",
        "// 32 glyphs * 56 entries (7 cols * 8 rows) = 1792 bytes.",
        c_byte_table("kBigFont", big, 16),
        "",
        "// fontes1517.gif baked into Java's 255x31 \"ac[]\" buffer. Values are",
        "// 1 (set) / 0 (transparent). Glyph i occupies ac[i*255 .. i*255+254]",
        "// stored row-major 15 cols * 17 rows; 31 glyphs total = 7905 bytes.",
        c_byte_table("kSmallFont", small, 32),
        "",
        "// 1-bit packed banner mask (Scrolls88's c[413]), unpacked at runtime",
        "// into d[3300] for the q() overlay pass.",
        c_byte_table("kBannerPacked", c_table, 16),
        "",
        "// Palette ramp table (Scrolls88's a[186]). Used to fill ap/ao/an",
        "// at indices 64..123 (60 entries * 3 channels = 180).",
        c_int_table("kPaletteRamp", a_table, 12),
        "",
        c_string("kTextBig", texts.get("bt", "")),      # i.bt — big scroller
        c_string("kTextBuLarge", texts.get("bu", "")),  # i.bu — horizontal small scroller
        c_string("kTextBsVert", texts.get("bs", "")),   # i.bs — vertical small scroller
        c_string("kTextBvBig", texts.get("bv", "")),    # i.bv — megadist big text (eqx=2)",
        "",
        "}  // namespace oldschool",
        "",
    ]

    with open(HEADER_PATH, "w") as out:
        out.write("\n".join(sections))
    print(f"Wrote {HEADER_PATH} ({os.path.getsize(HEADER_PATH)} bytes)")
    print(f"  big font:   {len(big)} bytes")
    print(f"  small font: {len(small)} bytes")
    print(f"  banner:     {len(c_table)} bytes")
    print(f"  palette:    {len(a_table)} int16 entries")
    print(f"  texts:      bt={len(texts.get('bt',''))}, bu={len(texts.get('bu',''))}, "
          f"bs={len(texts.get('bs',''))}, bv={len(texts.get('bv',''))}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
