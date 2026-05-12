# Oldschoolintro (Scrolls88) — 1:1 port spec

## Source

- `https://equinox.planet-d.net/java/oldschoolintro/` — applet `<applet code="Scrolls88.class" archive="Scrolls88.jar" width=320 height=196 codebase="."><param name=eqx value="1"></applet>`
- Author: Keops (Equinox), 2000-09-28.
- JAR is byte-identical to `https://equinox.planet-d.net/java/megadist/Scrolls88.jar`. The two demo URLs differ only in the `eqx` applet param (1 = oldschoolintro, 2 = megadist). **This port covers `eqx=1` only**; megadist (`eqx=2`) is a separate effect (giant single distorted scroller with eqxlogo banner) and will be a follow-up port that reuses the same class.
- Decompiled with CFR 0.152 → `decompiled/Scrolls88.java` (1991 lines) + `decompiled/a.java` (the four scroller texts).

## Canvas

- Internal: 320 × 200 (paletted, 256-color RGB565 via `ap/ao/an` per-index).
- Applet shows top 196 rows of the 200-row canvas.
- Panel mapping (handled by `DisplayManager::renderOldschoolIntroFrame`):
  - 640 × 172 panel (native landscape).
  - 2× horizontal scale (`srcX = logicalX >> 1`).
  - Vertical squash 200 → 172 via precomputed `srcYForCol[172] = (col * 200) / 172`.

## Asset decoding (`c(int=-1, String)` decoder, see line 1417)

The four image filenames are XOR'd character-by-character with `+4 - (i % 4)`. Decoded:

| Field | Encoded | Decoded | Size | Used for |
|-------|---------|---------|------|----------|
| `br`  | `bllsap57*dge` | `fontes78.gif` | 7×320 | Big-scroller font — 40 glyphs × 7 wide × 8 tall, stride 56 |
| `bq`  | `bllsap/4-4,fec` | `fontes1517.gif` | 15×527 | Small-scroller font — 17 glyphs × 15 wide × 31 tall, stride 255 |
| `bp`  | `,/.^.+ehb` | `020_2.gif` | 1026×32 | Megadist big-scroller font (eqx=2 only) |
| `bo`  | `anvkkdm-cfd` | `eqxlogo.gif` | 80×196 | Megadist Equinox logo banner (eqx=2 only) |

`eqxlogo.gif` and `020_2.gif` 404 on the oldschoolintro/ path; fetch them from `megadist/` instead.

For `eqx=1` we only need **fontes78.gif** and **fontes1517.gif**.

## Glyph encoding

### Big font (`fontes78.gif` → `o[1792]` via `r()` at line 656)

Stored as a single 56×32 byte sheet. The Java loader is unusual: instead of preserving the source colors, it encodes each non-black pixel as `1 + (n2 / 7)` where `n2 ∈ [0, 56)`. That means glyph i (i ∈ 0..7) gets pixel value `i+1` for every "lit" pixel in its 7-pixel-wide column band. Black pixels store 0 (transparent).

In other words: the original image just describes mask shapes; the loader assigns each column-band a fixed palette index 1..8 (the per-glyph color comes from the gradient `ap[1..8] = 0/48`, `ao[1..8] = ramp`, `an[1..8] = ramp`).

The big scroller text `i.bt` is baked into a 16800-wide × 8-tall strip `q[]` at startup by `h()` (line 1181). Each character of `bt` maps to ASCII − 65, with substitutions:
- `' ' (32) → 91`
- `'.' (46) → 92`
- `'!' (33) → 93`
- `':' (58) → 94`
- `',' (44) → 95`
- `'-' (45) → 96`

(Note: characters beyond the 8-glyph font are reused by mapping `n2 = char_index % 8` — i.e., the font has 8 distinct glyphs but the band-index modulus gives a colour-stripe effect across the text. See `h()` carefully.)

### Small font (`fontes1517.gif` → `ac[7905]` via `p()` at line 724)

Stored as a 255-wide × 31-tall byte sheet (the GIF's 15 × 527 pixels are reinterpreted as 255 × 31). Each value is 1 if non-black, 0 if black. 17 glyphs × 15 wide.

Substitutions used by the small-scroller draw routines `e()` / `g()`:
- `' ' → 91`, `'.' → 92`, `'!' → 93`, `\'\\'\' → 94`, `',' → 95`.

## Frame composition (one call to `v()`, line 498)

```
ay[64000] is the 320×200 paletted framebuffer (final output).
```

1. **Big X-distorted scroller** (lines 502–517 for `a7==1`):
   - For `n2 = 0..21` (22 vertical strips):
     - For `n = 0..7` (8 rows per strip):
       - Copy 320 bytes from `q[n*8400 + s[t++]]` to `ay[320 * (n + 9*n2 + 1)]`.
     - Copy `ak[0..319]` to `ay[320 * (8 + 9*n2 + 1)]` (separator row, normally 0 / transparent).
   - Plus `arraycopy(ak, 0, ay, 0, 320)` for screen row 0.
   - `t -= 174` after the strip; `t = 0` if `t > 12000`. Frame-to-frame increment is +2.
   - `s[24000]` is precomputed at init (line 313): `s[i] = 30 + (i/2) % 12200 + (int)(sin(i/30.0) * 30)`. The `(i/2) % 12200` term gives a steady leftward scroll, the sin term adds the wave.

2. **60-dot Lissajous ring** (lines 543–550):
   ```
   for n2 = 0..59:
     n  = cos(ab + (n2/60)*2π*2) * sin(v) * 20
        + cos((2*aa)/2 - (n2/60)*6.28) * 40
        - sin((aa/2 + z) + (n2/60)*2π) * 60 + 156
     n3 = cos((aa*1.5) + (n2/60)*2π) * sin(z) * 20
        - sin((4*ab)/3 + (n2/60)*6.28*2) * 40
        + sin((ab/2 + v) + (n2/60)*2π) * 20 + 100
     a(ay, n + n3*320)    // stamps a small fixed sprite at (n, n3)
   ab += 0.02; aa += 0.0146; z += 0.0182; v += 0.0206
   ```
   `a(byte[], int)` (line 1931) writes a hardcoded ~9×9 colored blob (palette indices 65–121) into `ay` at the offset. Center pixel is `byArray[5 + n] = 121`.

3. **Small scroller (top) bs** — `i()` at line 1143:
   - Renders 12 glyphs of `i.bs` near top of screen with vertical sine bob from `ad[]`.
   - Each glyph: `e(n, ah, charIdx*255 + 15, ay, ac)`.
   - `e()` (line 1338) writes a 15-wide × 16/17-tall glyph at base offset `1936 + (n*17 + ah)*320`, double-pumping rows (also writes `n10 + 272`, i.e., the row 17 cells below — produces the doubled height effect characteristic of this font). Color per row from `ag[]` (line 367, a small color ramp 25..49).
   - `ah` decrements by 2 each frame; when `≤ -16`, resets and advances `ai` (text cursor). Wave offset from `ad[ae++]` per glyph.

4. **Small scroller (middle) bu** — `k()` at line 923:
   - Renders 21 glyphs of `i.bu`, similar mechanism but with `g()` (line 1218) instead of `e()`.
   - Base offset `n5 = n * 16 + e` (i.e., starts at left edge, advances by 16 px per glyph horizontally).
   - Y-offset comes from `ad[ae++]`, glyph row gets color from `af[]` (line 363, sine bump 0..20 entries × ~20 height = small colour pop).
   - At the end, `q(ay, d)` overlays a 1-bit-per-pixel "EQUINOX" frame graphic decoded from `c[413]` (the constructor byte data) into `d[3300]` via `s(c, d)` at init. The overlay writes color 64 over any non-zero d byte.

5. The applet's outer `update()` draws `a0` (a MemoryImageSource wrapping `ay`) to the canvas at full size.

## State variables (oldschoolintro / eqx=1 subset)

| Java | Purpose |
|------|---------|
| `ay[64000]`     | Main paletted framebuffer (output) |
| `ak[64000]`     | Background/separator row source (mostly 0) |
| `q[134400]`     | Big-scroller pre-rendered strip (8 rows × 8400 + headroom) |
| `s[24000]`      | Sine-modulated horizontal offsets (read sequentially with wrap) |
| `t`             | Index into `s[]` (advances by 8*22 per frame, then -174) |
| `ab/aa/z/v`     | Lissajous phases for the 60-dot ring |
| `ac[7905]`      | Small-font mask (255×31) |
| `o[1792]`       | Big-font mask (56×32) |
| `ad[1002]`      | Sine-bob LUT for small-scroller glyph y-offsets |
| `af[200]`       | Color-bump LUT for `k()` glyphs |
| `ag[200]`       | Color-bump LUT for `i()` glyphs |
| `d[3300]`       | Decoded 1-bit overlay mask |
| `e, f`          | `k()` glyph x-offset and text cursor |
| `ah, ai`        | `i()` glyph x-offset and text cursor |
| `ae`            | `ad[]` cursor for both small scrollers |
| `ap/ao/an[256]` | R/G/B per palette index |

## Frame timing

Java run loop sleeps `as` ms (auto-tuned 1..14 ms by `w()`). For ESP32-S3 we drive at the panel rate (~60 fps); each `tick()` does one `v()` step. The Java code's runtime FPS auto-tuner is dropped.

## Native-stripe panel rendering

`Pupul` precomputes `srcYForCol[172]` and stripes `txBuffer_` in 47-row chunks. Same pattern here:

- Allocate `frame_` = 320 × 200 RGB565 in PSRAM (≈ 125 KB).
- After `tick()` writes the 64000 indexed pixels, **convert in place** via `ap/ao/an → RGB565` LUT into a parallel RGB565 buffer (or convert on-the-fly inside the stripe builder — depends on memory pressure).
- For each native stripe `s = 0..3` (47 rows tall on the 640 dim):
  - For each panel column `c = 0..171`:
    - `srcY = srcYForCol[c]` (0..199)
    - For each panel row in this stripe (47 rows on the 640 dim):
      - `srcX = (s*47 + row) >> 1`  (0..319)
      - Write `frame_[srcY*320 + srcX]` (RGB565) into `txBuffer_[c*47 + row]`.
  - `display.drawBitmap(stripeX, 0, txBuffer_, 47, 172)` — same axs15231b mapping Pupul uses.

Memory: 320 × 200 × 2 = 125 KB (RGB565 frame); same for the indexed buffer (uint8_t, 64 KB) → ~190 KB PSRAM, comfortably within budget.

## Palette construction

Built once in `begin()`. Mostly procedural (per-index gradients); palette indices 64..123 come from a baked `int a[186]` table embedded in the constructor (lines 1486–1928). After building per-index `R/G/B` bytes, convert to RGB565 LUT once.

## Scroller texts (from `a.java`)

The four texts (`bt`, `bu`, `bs`, `bv`) are copy-pasted verbatim into our `OldschoolIntro.cpp`. For `eqx=1`, only `bt`, `bu`, `bs` are displayed; `bv` is unused in this mode but kept for the megadist follow-up.

## Verification anchors

The Java code has integrity checks via four computed checksums (`bi`, `bh`, `bg`, `bf`) summed from char codes of the text. These gate the rendering branches. For our port we just always render — the checks are anti-tamper of the original applet and not relevant here.
