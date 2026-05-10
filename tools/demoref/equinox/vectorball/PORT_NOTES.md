# Vectorball — port notes

Reverse-engineered from CFR-decompiled `Vectorball.java` (entry point) and
`a.java` (renderer) from the original Equinox 1992 Atari ST applet by Keops.
This is a spec for a 1:1 visual port — no C++ code yet.

Two run modes are wired in the source (`eqx` applet param):
- **eqx=1** — the canonical "vectorball" mode (300×280, 400 balls, sphere
  sprites, palette of 136 RGB triples). All sections below describe this mode.
- **eqx=2** — a degenerate "lores" debug mode (25 balls, line-rendered).
  Same math, simpler sprite — ignore for the visual port.

## Constants worth reusing

| symbol             | value                | meaning                                |
|--------------------|----------------------|----------------------------------------|
| `SCREEN_W`         | 300                  | framebuffer width (`ag`)               |
| `SCREEN_H`         | 280                  | framebuffer height (`af`)              |
| `NUM_BALLS`        | 400                  | `aw` / `ab`                            |
| `FOCAL`            | 300.0                | perspective focal length               |
| `CAMERA_Z`         | -1100                | `a8`; ball Z is offset by this         |
| `Y_OFFSET_BASE`    | 891                  | initial `a3` (Y screen-center bias)    |
| `SPRITE_W`         | ~15                  | sprite footprint, columns              |
| `SPRITE_H`         | 15                   | sprite footprint, rows                 |
| `PALETTE_ENTRIES`  | 136                  | of 256 LUT slots; rest is ramp filler  |
| `TARGET_SLEEP_MS`  | 1                    | initial; auto-tuned 1..14 ms (see *Timing*) |

## Ball positions

There is **no single "uniform unit sphere"**. `a.g(int)` precomputes
**10 different shape sets** of 400 points each, stored in 10 parallel
`int[400]` triplets (X/Y/Z): `(cn,cm,cl)`, `(ck,cj,ci)`, `(ch,cg,cf)`,
`(ce,cd,cc)`, `(ca,b9,b8)`, `(b7,b6,b5)`, `(b4,b3,b2)`, `(b1,b0,b_)`,
`(bz,by,bx)`, `(bw,bv,bu)`. The runtime morphs from one set to the next
(see *Rotation/animation*) so the cloud reshapes continuously.

Let `ay = sqrt(400) = 20` (used as a 20×20 grid index for the first three
shapes). All shapes are scaled into roughly ±400 model-space units.

```
// Shape 0 — "wave plane" (sin·cos grid), eqx=1
for (i=0; i<20; i++)
  for (j=0; j<20; j++) {
    cn[i+j*20] = (int)(400*i/20 * 2.2 - 440);              // X
    cm[i+j*20] = (int)(400*j/20 * 2.2 - 440);              // Y
    cl[i+j*20] = (int)(cos(6.283/20 * i * 2) *
                       sin(3.14159/20 * j * 2) * 400/5);   // Z
  }

// Shape 1 — quartic-saddle plane
ck[i+j*20] = 400*i/20 * 2 - 400;
cj[i+j*20] = 400*j/20 * 2 - 400;
ci[i+j*20] = (i-10)*(i-10)*(j-10)*(j-10) * 400 /
             (20*20*20*20/16) - 200;

// Shape 2 — saddle + cosine bump
ch[i+j*20] = 400*i/20 * 2 - 400;
cg[i+j*20] = 400*j/20 * 2 - 400;
cf[i+j*20] = (int)(-(i-10)*(i-10)*(j-10)*(j-10) * 400 /
                   (20*20*20*20/16)
                   + cos(1.2566 * i*j) * 20*20/2.0
                   + 160);

// Shape 3 — Lissajous-ish blob (n=ball index, k=eqx; k=1 → mul=1)
cc[n] = (int)(cos(0.069811 * n) * sin(0.10472  * n) * 400 * 0.9);
ce[n] = (int)(sin(0.089757 * n) * sin(0.062832 * n) * 400 * 0.9);
cd[n] = (int)(sin(0.125664 * n)                     * 400);

// Shape 4 — torus-ring (sin spiral); n in 0..399
ca[n] = (int)(cos(0.6283  * n) * sin(6.28318/400 * n) * 400);  // X
b8[n] = (int)(sin(0.6283  * n) * sin(6.28318/400 * n) * 400);  // Z
b9[n] = 800/400 * n - 400;                                     // Y (linear)

// Shape 5 — half-frequency variant (taller spiral)
b7[n] = (int)(sin(0.6283  * n) * sin(6.28318/400 * n / 2) * 400);
b5[n] = (int)(cos(0.6283  * n) * sin(6.28318/400 * n / 2) * 400);
b6[n] = (int)(800/400 * n * 1.5 - 600);

// Shape 6 — same shape, slightly different spin (0.61598 vs 0.6283)
b4[n] = (int)(sin(0.61598 * n) * sin(6.28318/400 * n / 2) * 400);
b2[n] = (int)(cos(0.61598 * n) * sin(6.28318/400 * n / 2) * 400);
b3[n] = (int)(800/400 * n * 1.5 - 600);

// Shape 7 — 1.5x freq spiral
b1[n] = (int)(cos(0.6283 * n) * sin(6.28318/400 * n * 1.5) * 400);
b_[n] = (int)(sin(0.6283 * n) * sin(6.28318/400 * n * 1.5) * 400);
b0[n] = 800/400 * n - 400;

// Shape 8 — tight 10-loop spiral (X linear)
bx[n] = 400*n/400 * 2 - 400;
bz[n] = (int)(cos(6.283/400 * n * 10) * 400);
by[n] = (int)(sin(6.283/400 * n * 10) * 400);

// Shape 9 — pure random cloud, ±300
bu[n] = (int)(600*Math.random() - 300);
bw[n] = (int)(600*Math.random() - 300);
bv[n] = (int)(600*Math.random() - 300);
```

For the simplest port that *looks* like Vectorball, **Shape 4 (`ca/b9/b8` —
the spiraled ring)** is the most iconic choice. For a faithful port,
implement all 10 and morph between them as below.

## Rotation / animation timeline (`w()` + `j()` + `l()`)

A single global frame counter `ba` increments each frame. The 10 shapes are
shown in sequence with linear morphs between consecutive shapes. Cumulative
frame thresholds live in `bf[0..18]`, computed from `bg[0..18]`:

```
bg = { 1000, 100, 200, 100, 200, 100, 200, 100, 200, 100,
        200, 100, 150, 100, 200, 100, 200, 100, 200 };
bf[0]=1000, bf[1]=1100, bf[2]=1300, bf[3]=1400, bf[4]=1600,
bf[5]=1700, bf[6]=1900, bf[7]=2000, bf[8]=2200, bf[9]=2300,
bf[10]=2500, bf[11]=2600, bf[12]=2750, bf[13]=2850, bf[14]=3050,
bf[15]=3150, bf[16]=3350, bf[17]=3450, bf[18]=3650
```

So roughly: shape held for 1000 frames, then 100-frame morph, then next
shape for 200 frames, etc. The cycle restarts at frame 3650.

`o(X,Y,Z)` does a static rotation+translate of one shape; `m(Xa,Ya,Za,Xb,Yb,Zb,k)`
linearly interpolates between two shapes by `f = (ba - bf[k*2]) / 100.0`.

### Rotation matrix (`l()` — Tait-Bryan ZYX, then negated)

Three Euler angles `a6, a4, a9` are accumulated. Matrix coefficients
written into 9 floats `ar..aj`:

```
f1 = cos(a6); f2 = sin(a6);
f3 = cos(a4); f4 = sin(a4);
f5 = cos(a9); f6 = sin(a9);

ar =  f1*f3
aq = -(f1*f4*f6 + f2*f5)
ap = -(f1*f4*f5 - f2*f6)
ao =  f2*f3
an = -(f2*f4*f6 - f1*f5)
am = -(f2*f4*f5 + f1*f6)
al =  f4
ak =  f3*f6
aj =  f3*f5
```

Per ball:
```
xr = ar*x + aq*y + ap*z          // rotated X
yr = ao*x + an*y + am*z + a3     // rotated Y + Y offset
zr = al*x + ak*y + aj*z + a8     // rotated Z + camera Z (-1100)
```

### Per-frame angle update (`j()`)

A small DSL drives the rotation: `bd[8]` durations + `bc[24]` (= 8 × XYZ)
**rotation rate triplets**, expressed in degrees-per-frame and converted via
`* 0.017453` (π/180). The 8 segments cycle:

```
//          frames   d_a6        d_a4        d_a9      (deg/frame on X,Y,Z)
seg 0: { 150,  0.333333,  0.666667,  0.000000 },
seg 1: { 100,  0.666667,  0.666667,  0.666667 },
seg 2: { 100,  0.666667,  1.000000,  0.666667 },
seg 3: { 100,  0.666667,  0.333333,  0.666667 },
seg 4: {  50,  0.666667,  0.666667,  1.333333 },
seg 5: {  50,  0.666667,  1.000000,  1.333333 },
seg 6: { 150,  0.666667, -0.666667,  1.000000 },
seg 7: { 100,  0.666667, -0.333333,  0.666667 }
```

Each frame:
```
a6 += 0.017453f * dx[seg]
a4 += 0.017453f * dy[seg]
a9 += 0.017453f * dz[seg]
if (++a1 >= bd[seg]) { a1=0; if (++a0 >= 8) a0=0; }
```

### Y bobbing (`a3`)

In eqx=1 mode `a3` (Y screen-center bias added inside `o()`/`m()`)
ramps from initial 891:

```
if (ba < 160)         a3 -= 6;     // dive in
if (ba > bf[18]-160)  a3 += 6;     // climb out
```

…so the ball cluster slides down into view, sits centered, slides off,
then loops.

## Projection (`v()` / `u()`)

After rotation each ball has model-space coords `(X, Y, Z)`. Projection is
classic perspective with focal=300 and the **near-plane band** `[a8-600 .. a8+600]`
used for the depth-cue bucket:

```
near = a8 - 600;           // a8 = -1100, so near = -1700
far  = a8 + 600;           // -500
step = (far - near) / 6;   // 200, six brightness buckets

for each ball n:
    z = Zr[n];
    f = 300.0 / (300.0 - z);     // perspective scale
    sx = (int)(Xr[n] * f + 150.0 - 7.0);   // 7 = sprite half-width-ish
    sy = (int)(Yr[n] * f + (af/2 - 7));
    // depth bucket → sprite brightness 1..6 (6 = nearest):
    bucket = z<near+1*step?5 : z<near+2*step?4 :
             z<near+3*step?3 : z<near+4*step?2 :
             z<near+5*step?1 : 0;
    drawSprite(framebuffer, sx + 300*sy, bucket+1);
```

Clamps used: `sx` to `[0, 284]`, `sy` to `[0, af-16]` so the 15×15 sprite
never wraps. The `+150 - 7` sub-expression centers screen X (`SCREEN_W/2`)
and biases by half the sprite. Note Y center bias `af/2 - 7 = 133` (with a
separate `+a3` already injected during rotation).

In the eqx=2 path the same math runs except focal=300 hardcoded
differently (`(300 - Z/2)` scale) — ignore.

## Palette (136 RGB triples, then 256 grayscale tail)

`Vectorball.b()` populates LUTs `t[256]` (R), `s[256]` (G), `q[256]` (B).
For eqx=1 the **first 136 slots** are loaded straight from
`Vectorball.this.d[3*n .. 3*n+2]` (the 408-int list in the constructor).
Slots 136..255 are left at zero. (For eqx=2 it builds a procedural blue
ramp instead — irrelevant to the canonical look.)

The palette has **two functional halves**:

- **Index 0** — pure white (255,255,255), used as the marker color the
  sprite layer matches against (see `t()` clearing logic).
- **Indices 1..63** — the **cool sphere ramp** (white → pink → blue →
  near-black). This is what `f()` writes for the sphere disc.
  Pseudocode `f()` only ever writes values 1..63 into the framebuffer.
- **Indices 64..127** — overflow / extra colored ramps (used by case-2
  variants and the secondary effect plate).
- **Index 128 / 129..** — used as the "background" plate value
  (Vectorball.init writes `n4=128` then 129..135 for the bottom 7 lines).
- **Indices 195..387 in `d[]`** (i.e. palette slots 65..128) — a *second*
  copy of the same ramp tinted with stronger blue/orange casts (used by
  the eqx=2 mode's brighter rendering pass).

Practical port: only LUT entries 0..63 matter for the visible balls.
Entries 1..63 form a 63-step gradient from pale-white (1: 255/222/247)
through pinky-mauves (~10: 181/206/231), through mid blues (~30:
148/156/189), down to near-black blue (~60: 8/74/90). Slot 64 = white
again starts the next ramp.

A practical fast extraction: parse the `nArray2[i] = N;` lines from
`Vectorball()` into `byte d[408]`, then `palette[n] = (d[3n], d[3n+1], d[3n+2])`
for n = 0..135. (Three indices in `d[]` are deliberately unused — `nArray2[112]`,
`nArray2[121]`, `nArray2[124]` etc. are left zero. Treat zero R,G,B as
"transparent black" for those slots.)

Below is the first 32 entries (the part the sprite samples) decoded by hand
from the constructor body — full table (136 entries) is mechanical to
extract from `Vectorball.java` lines 367..712:

```
 0: (255,255,255)  16: (148,206,222)
 1: (255,222,247)  17: (148,189,214)
 2: (255,214,231)  18: (165,189,214)
 3: (247,181,222)  19: (132,189,214)
 4: (239,198,222)  20: (123,189,206)
 5: (239,165,206)  21: (132,181,206)
 6: (231,181,206)  22: (107,181,198)
 7: (231,148,206)  23: (198,198,198)
 8: (222,148,189)  24: (148,173,198)
 9: (214,148,189)  25: (115,173,198)
10: (214,132,189)  26: (107,173,189)
11: (214,123,189)  27: (115,165,189)
12: (206,132,181)  28: ( 99,156,189)
13: (206,107,181)  29: ( 90,165,173)
14: (198,198,198)  30: ( 99,148,173)
15: (198,148,173)  31: ( 82,140,173)
                   …continues to ~( 8, 74, 90) at idx ~57…
```

(Author note for the porter: load all 136 triples into a `static const
uint8_t kVectorballPalette[136][3]` table at port time — it's just data.)

## Sprite / per-ball draw (`f()`)

There is **no shaded-sphere generator at runtime**. The author hand-encoded
**6 pre-shaded sphere discs** at brightness levels 1..6 (1 = darkest /
furthest, 6 = brightest / nearest). Each disc is a 15-row × ~15-column
silhouette stamped directly into the 8-bit framebuffer.

Layout per disc (writes into `byArray[n + offset]`):

| row | y-stride offset | column range | width |
|-----|-----------------|-------------|-------|
|  0  |    5..9         | x=5..9      |  5    |
|  1  |  303..311       | x=3..11     |  9    |
|  2  |  602..612       | x=2..12     | 11    |
|  3  |  901..913       | x=1..13     | 13    |
|  4  | 1200..1213      | x=0..13     | 14    |
|  5  | 1500..1514      | x=0..14     | 15    |
|  6  | 1800..1814      | x=0..14     | 15    |
|  7  | 2100..2114      | x=0..14     | 15    |
|  8  | 2400..2414      | x=0..14     | 15    |
|  9  | 2700..2714      | x=0..14     | 15    |
| 10  | 3000..3013      | x=0..13     | 14    |
| 11  | 3301..3313      | x=1..13     | 13    |
| 12  | 3602..3612      | x=2..12     | 11    |
| 13  | 3903..3911      | x=3..11     |  9    |
| 14  | 4205..4209      | x=5..9      |  5    |

(Row stride = 300 = `SCREEN_W`. Note row-y 11 starts at 3301, not 3300 —
the author intentionally jitters a couple of rows by ±1 column to get a
slightly irregular sprite outline; preserve the offsets exactly.)

The pixel values written are **direct palette indices** in 1..63. Case 1
uses values mostly in [1..57], case 2 in [2..61], case 3 in [3..62],
case 4 in [4..62], case 5 in [7..63], case 6 in [15..63]. Same shape, same
gradient layout, but the values are scaled up so the disc looks brighter
and more white-cored as you go nearer (cases 5,6 saturate at 63 at the
silhouette edge to give the bright-rim look).

**Just copy the literal arrays from `f()` cases 1..6** into 6×181 byte
tables at port time — that's the entire sprite asset.

The sprite blits **without alpha** — it overwrites whatever was in the
framebuffer. Since the painter sort goes back-to-front (see below), the
front balls stamp on top.

## Painter sort (`r()` — bucket sort by Z)

Per frame, after rotation but before drawing, balls are sorted by Z so
the renderer iterates back→front:

```
// bi[] is a 1600-entry "buckets initialized to 0" template, copied each frame
arraycopy(bi, 0, bj, 0, 1600);           // clear buckets

for (n = 0; n < 400; n++) {
    z   = Zr[n];                          // post-rotation Z
    bin = z - a8 + 800;                   // a8=-1100 → bin in roughly [200..1400]
    while (bj[bin] != 0) bin++;           // linear-probe past collisions
    bj[bin] = n + 1;                      // store ball index +1 (0 = empty)
}

// Compact non-zero entries into bh[] in Z order:
out = 0;
for (i = 0; i < 1600; i++)
    if (bj[i] != 0) bh[out++] = bj[i];

// Then draw loop iterates bh[] front-to-back? No — sprite draw uses
//   for (k=0; k<400; k++) drawBall(bh[k]-1);
// which paints in ascending Z bucket order. Since a8=-1100 and far is more
// negative, bins fill back-to-front: index 0 = farthest. That's painter's
// algorithm.
```

The "sort" is therefore an **O(N) bucket sort with linear collision probing**
into a 1600-slot array. There's no qsort or insertion sort. Bucket size = 1
Z unit. Run every frame.

## Timing (`Vectorball.run()` + `e()`)

Main loop:
```
while (running) {
    Thread.sleep(this.w);           // w starts at 1L
    d();                            // clear framebuffer (memcpy bg plate)
    ac.s(...);                      // animate + project + sort + draw
    ad.newPixels(...);              // push to image source
    repaint();
    e();                            // adapt 'w' so framerate ~60–80 fps
}
```

`e()` measures frames-per-second over a 1-second window and adjusts
`w` (sleep ms) within `[1..14]`. Roughly:

| measured FPS | adjustment        |
|--------------|-------------------|
| ≤ 10         | w -= 4            |
| 11..20       | w -= 3            |
| 21..30       | w -= 2            |
| 30..60       | w -= 1            |
| > 80         | w += 1            |

So the **target framerate is ~60..80 fps**; on a fast modern host the
adapter will drive `w` toward the upper clamp of 14ms (~71 fps). For the
firmware port: just lock to **70 fps (≈14 ms/frame)**.

Frame counter `ba` (used to drive shape morphing in `w()`) increments
every frame regardless of wall clock — so the motion timing is in
**frames, not seconds**. At 70 fps the full 3650-frame loop runs ~52 s.

## Open questions / things the obfuscation hides

1. **`a.x()` (eqx=2 only)** — builds two procedural sprites (cosine and
   `Math.exp` in the y-coord!) into `bl[2304]`. The exp() call is almost
   certainly a Jad/CFR mis-decompile of `Math.sin` (the author warned us:
   "Are you a Jad/Decafe lamer?"). Doesn't affect eqx=1 visuals — skip.

2. **`p()` background scroller** — uses `av[720]` (a doubled cos LUT,
   built in `q()`) to produce a **horizontal sine-wave warp on the bottom
   1/5th of the screen**, drawn after balls. Reads source row `n4`, writes
   to row `n5`, x-shifts by `av[n6]` per row, OR-ing 0x40 into the byte to
   recolor it. This is the wavy "scroll deck" under the ball cloud. Look at
   `p()` lines 573-605 if you want it; it's optional polish.

3. **`a.r()` collision probing** — the decompiler couldn't structure the
   `while (bj[bin] != 0) bin++` inner loop. Bytecode confirms the layout
   above (the goto labels `lbl-1000`, `lbl10`, `lbl11` are just the loop
   back-edge and break).

4. **`Vectorball.d()`** — for eqx=1 just memcpys 4/5 of the background
   plate (`o`) over the framebuffer (`ae`). For eqx=2 it does an "age the
   pixels by -20" trail effect. Decompiler bailed on the `goto lbl22`,
   but the bytecode is just `do { if (ae[i] > 19) ae[i] -= 20; } while (++i < n);`

5. **The `at[4][10]` matrix** — only ever written, never read in any
   frame-output path. It's a deliberate red herring (look at the `Math.log`,
   `Math.exp`, redundant trig calls scattered through `j()`, `l()`,
   `m()`, `r()`, `i()`). Ignore it entirely. The actual rotation state is
   only `(a6, a4, a9)` and the matrix `(ar..aj)`.

6. **Color of each ball** — every ball uses the *same* sphere sprite
   (one of 6 brightness levels chosen by Z bucket). There is **no
   per-ball base color**. The "colors" in the cloud come purely from the
   palette ramp values 1..63 baked into the 6 sprite bitmaps. This is
   simpler than the prior decompile pass guessed.

7. **Three deliberately-zero palette slots** (`nArray2[112], [121], [124]`
   etc.) are unused 0,0,0 entries — they are real holes in `d[]`,
   not decompiler artifacts. Initialize them to black; the sprite never
   indexes them.
