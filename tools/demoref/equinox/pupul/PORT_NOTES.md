# Pupul — port notes

Reverse-engineered from CFR-decompiled `Pupul.java` (entry point / renderer)
and `a.java` (loader thread) of the Equinox 1989 Atari ST intro re-released
as a 2000-era Java applet. This is a spec for a 1:1 visual port — no C++
code yet. Implementation lands in a subsequent OTA after the spec is
agreed on.

Live applet: <https://equinox.planet-d.net/java/pupul/> (assets pulled into
`assets/`; that directory is the canonical source for the port).

## Assets (already extracted into `assets/`)

| file              | dimensions | purpose                                    |
|-------------------|------------|--------------------------------------------|
| `fondpupul2.gif`  | 320 × 196  | main background — sky / mountains / temple |
| `fondpupul22.gif` | 320 × 86   | "raster strip" graphic — slides into bg    |
| `dragon1.gif` … `dragon8.gif` | 32 × 26 | dragon animation frames (8) |
| `fonte.gif`       | 320 × 78   | scroller font sheet (3 rows × 10 cells)    |

Java loader order (see `Pupul.d()`): `fondpupul2`, `dragon8` → `dragon1`,
`dragon2`, `dragon3`, `fonte`, `fondpupul22`. The 8 dragon GIFs are kept as
independent `Image[] a_[1..8]` slots; `a_[0]` is reserved for the background
buffer (`MemoryImageSource` over the `ad[]` int array).

## Canvas + panel mapping

The applet is **320 × 228**:
- y=0..195: background area (`fondpupul2`, painted into `ad[]` int[] of size
  `320 × 196`).
- y=196..227: scroller strip (32-px tall, 11 sprite cells × 32 px wide).

Our panel is **640 × 172** (logical). Plan:
- **Horizontal:** 2× scale (each source column ↦ 2 panel columns) so dragons /
  raster bars / text glyphs read at native chunkiness, not micro-shrunk.
- **Vertical:** map panel y∈[0,172) ↦ source y∈[0,228) via `srcY = panelY * 228 / 172`
  (≈ 0.75× squash). Drops ~25% of vertical resolution — looks like a wide-
  pixel CRT, consistent with the Atari ST aesthetic.
- Implementation: render the demo into an internal 320×228 RGB565 buffer per
  frame, then blit to the panel with the above mapping inside a native-stripe
  loop (compose stripes directly into `txBuffer_` in panel-native orientation,
  same pattern as `renderModulePlayerFrame`).

Total internal buffer: 320 × 228 × 2 B = **142.5 KB**. Lives in PSRAM
(internal RAM stays free for the SPI `txBuffer_` pair).

## Asset baking

Assets are 8-bit palette-indexed GIFs. Keep them as **indexed + palette** in
the embedded headers (1 B/pixel + 256×2 B palette per asset) — about half the
size of plain RGB565. Sizes:

| asset             | bytes (indexed) | bytes (RGB565) |
|-------------------|-----------------|----------------|
| fondpupul2        | 62 720          | 125 440        |
| fondpupul22       | 27 520          |  55 040        |
| dragon1..8 (each) |     832         |   1 664        |
| fonte             | 24 960          |  49 920        |
| **Total**         | **≈ 122 KB**    | **≈ 244 KB**   |

Build-time conversion: a Python script (`tools/convert_pupul_assets.py`,
to be written) reads each GIF, emits a header (`pupul_assets.h`) with:

```cpp
struct PupulIndexedSprite {
  const uint8_t *pixels;   // 8-bit palette indices, row-major
  const uint16_t *palette; // 256 × RGB565
  uint16_t width;
  uint16_t height;
};
extern const PupulIndexedSprite kPupulFondMain;       // fondpupul2
extern const PupulIndexedSprite kPupulFondStrip;      // fondpupul22
extern const PupulIndexedSprite kPupulDragonFrames[8];
extern const PupulIndexedSprite kPupulFont;
```

Use 0x0000 (black, palette index 0 in source GIFs) as the dragon transparent
key — the original Java relies on GIF alpha which Java's `Image` auto-honours;
we re-create that with a "skip 0-index pixels when blitting dragons" rule.

## Frame composition (per `Pupul.b()`)

Once per frame the renderer does the following, in order:

1. **Restore previously-XOR'd raster regions.** For each of 10 raster bars
   (`z[0..19]` pair-indexed), if its top `z[i*2+1]` < bottom `z[i*2]`, the
   pixel rows in that range were inverted (`xor 0xFFFFFF`) last frame.
   Copy the pristine source background `ac[]` (saved at init from `ad[]`)
   back into `ad[]` for those rows. Effectively *un-paints* the bars.

2. **Advance raster-bar phase + recompute Y positions.** For each of the
   10 bars:
   ```
   v[i*2]   -- 1    // bottom speed
   v[i*2+1] -- 1    // top speed
   if (v[i*2+1] <= 0) v[i*2]=95, v[i*2+1]=100   // reset pair
   if (v[i*2]   <= 0) v[i*2]=1
   z[i*2]   = 296 / v[i*2]   + 110   // bottom Y
   z[i*2+1] = 296 / v[i*2+1] + 110   // top Y
   ```
   This produces 10 phase-offset horizontal bars that scroll up the screen
   (decreasing `v` ⇒ increasing `z` ⇒ Y drops, but the geometry is a slow
   crawl through y≈110 → 196 as `v` ticks down from 100..1).

3. **Paint `fondpupul22` strip into `ad[]`** at each active bar row. The
   strip image (320×86) is sampled with `srcRow = barRow - 110` so the
   raster scroll reveals a different chunk of the strip per bar; copies
   `n × 320` pixels per bar where `n` is the bar's visible height.

4. **Clear bottom border** — `aa[]` (a 320-px row of `-4482748` ≈ a teal
   gradient pixel) is copied to row 195 of `ad[]` (offset 195*320 = 62 400).
   Looks like a thin baseline under the temple background.

5. **Push `ad[]` to the screen** via `s[11].newPixels(0, 111, 320, 85)` →
   `drawImage(a_[0], …, 0,73, 320,200, …)`. Equivalent to: after compositing
   into `ad[]`, blit the (top-clipped) background to the canvas.

6. **Advance dragon animation state.** State machine on `an`:
   - `an=0` (idle, holds on frame `aj=1`). After 250 frames, switch to
     `an=2`, set `am=1` (animation direction).
   - `an=2` (animate 1→8 then 8→5 bounce): every even `ar`, `aj += am`.
     When `aj==8`, flip `am=-1`. When `aj==5`, flip `am=+1`. After 400
     frames, switch to `an=3`.
   - `an=3` (wind down): every even `ar`, `--aj`. When `aj<=1`, switch to
     `an=0`, `++g`, reset `ar=0`.
   - `aj` is the current sprite index (1..8 = `a_[1..8]`).

7. **Draw the dragon (7 + 1 = 8 copies)**, each at a different `(u, t)`
   path position. There are 951 precomputed positions in `u[]` and `t[]`,
   each segment using a different angular speed (see *Path arrays* below).
   The original loop draws the main dragon + 6 trailing offsets:
   ```
   drawImage(a_[aj], u[ap + n2*16], t[ao + n2*16], 32, 26)  // n2=0..6
   drawImage(a_[aj], u[ap],         t[ao],          32, 26)  // main
   ```
   `ap` and `ao` are independent counters that wrap at 939 and 950 — the
   dragons share `(u,t)` but with offset indices, producing a 7-comet trail.

8. **Scroll text strip.** Render 11 sprite slots (`r[0..10]`), each a
   32×25 cell holding one glyph. Each frame:
   ```
   drawImage(r[n], af[n], 200, 32, 25)
   af[n] -= 6           // scroll left
   if (af[n] <= -32) {
     af[n] += 352       // wrap to right edge
     // fetch next char from the text string, blit from fonte.gif into ag[n]
     // see "Font decoding" below
   }
   ```
   The scroller text is the canonical:
   `"       HI KIDS! THIS IS THE GREAT PUPUL INTRO COMING BACK FROM AGES!!!  THIS JAVA VERSION IS NEARLY THE SAME AS OUR ORIGINAL ATARI INTRO CODED TEN YEARS AGO...  TIME TO WRAP!      "`
   (note the leading/trailing padding for clean entry/exit).

9. **"THE EQUINOX TOUCH" overlay.** Static text at (100, 57). Constant per
   frame. Source uses `Graphics.drawString` — we'll bake "THE EQUINOX TOUCH"
   into a small 1-bpp glyph row at load time, or just draw it from `fonte`
   character cells.

## Path arrays (`u[]`, `t[]`)

Pre-computed at init in `Pupul.c()`. Five sweeps with different angular
deltas produce a complex Lissajous curve:

| index range | `dα` (u, cos) | `dβ` (t, sin) |
|-------------|---------------|---------------|
| 0..124      | 0.05          | 0.055         |
| 125..219    | 0.03          | 0.035         |
| 220..479    | 0.06          | 0.03          |
| 480..629    | 0.045         | 0.035         |
| 630..939    | 0.02          | 0.045         |

Position formula (constant for all segments, `az = 32`, `aw = 26`):
```
u[i] = 160 - az/2 + (128 - az/2 - 5)  * cos(α)   ≈ 144 + 107 * cos(α)
t[i] = 98  - aw/2 + 30 + (65.33 - aw/2 - 10) * sin(β) ≈ 115 + 42.33 * sin(β)
```
So the dragon flies in roughly the ellipse `x∈[37,251], y∈[73,157]` over
its 940-step path. The trailing dragons sample 16-step offsets so they
form a comet streak in the same orbit.

## Font decoding (`fonte.gif` → 32×25 cells)

`fonte.gif` is 320×78. Logical layout: 3 rows × 10 columns of 32×26 cells
(`fonte` is only 78 tall = 3×26, but reads use 25-row cells with 320-px
stride). Character → cell offset:

```
char c (uppercase ASCII)
if (c == ' ' || c == '!' || c == '.' || c == '?')      → punctuation row
  '!' → offset 16832   (special row, col 0)
  '.' → offset 16864   (special row, col 1)
  '?' → offset 16896   (special row, col 2)
else c -= 65  ('A' = 0)
  if (c >= 20)  → offset = (c - 20) * 32 + 16640  (row 2: 'U'..'Z')
  elif (c >= 10) → offset = (c - 10) * 32 + 8320  (row 1: 'K'..'T')
  else           → offset = c * 32                (row 0: 'A'..'J')
```

Each "offset" is the top-left pixel index into the 320-wide font sheet.
The cell is then read 25 rows tall, copying 32 pixels per row with stride
320 into the destination sprite buffer `ag[n][32 × 25]`.

Out-of-range chars (digits, lower-case, etc.) get a 32×25 transparent cell
(`-16777216` = black with alpha 0xFF).

## Animation timing

Java source sleeps `Thread.sleep(10)` between frames. The applet aims for
~100 fps but the actual rate is limited by `repaint()` + `drawImage`
overhead — real-world ~30-40 fps on contemporary hardware. We should
target panel-rate (60 fps) with no compose/SPI overlap concerns since the
asset blits are small (320×196 background dominates; ~63 KB per frame).

State counters (`ar`, `ap`, `ao`, `q`, `g`) increment monotonically. `ar`
resets at state transitions; `ap`/`ao` wrap at 939/950; `q` wraps at end
of scroller string.

## Implementation TODO (for the next OTA)

1. Write `tools/convert_pupul_assets.py` → emits
   `src/demos/pupul_assets.h` (palette-indexed + RGB565 LUT).
2. Add `src/demos/Pupul.h` / `.cpp` modelling state (frame counter, raster
   `v[]` / `z[]`, dragon `an`/`aj`/`am`/`ap`/`ao`, scroller `af[]` /
   `q` / per-cell `ag[]`).
3. Add `DisplayManager::renderPupulFrame(const Pupul &p)` — native-stripe
   compose into `txBuffer_` from the internal 320×228 framebuffer using
   the panel mapping above.
4. Wire into `DemoKind` enum + `DemoPicker` + `enterDemoPlayback` switch.
5. Profile with `[saver]`-style timing logs; raster XOR + dragon blits
   are the hot paths.

## Notes / gotchas

- The 10 raster-bars XOR onto `ad[]` (the background int buffer), invert,
  paint, leave inverted. Next frame, restore `ac[]` (original) then re-XOR
  with new positions. Without the restore, inversions compound and the
  screen drifts to black. Our port has to mirror that two-phase pattern
  exactly or the visual is wrong.
- `fondpupul22` is 86 rows tall but is read with a `-35200` source offset
  (`= -110 * 320`) so it lines up under bar Y positions starting at 110.
  Implementation: when painting bar at row `r`, read source row `r - 110`
  from `fondpupul22`. Out-of-range = transparent.
- 7-trail dragon stamp uses the SAME `(u, t)` arrays as the main dragon
  but offset by `n2 * 16` indices. The trails overlap and the alpha
  blending in GIF gives a slight "ghost" effect — replicate by blitting
  with the GIF's transparent index (0) skipped (no alpha).
- "THE EQUINOX TOUCH" is drawn with Java's default font (`Color.white`,
  no specific font face). Use our `kTinyScale=2` font for the port —
  visually close enough.
