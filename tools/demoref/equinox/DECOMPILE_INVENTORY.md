# Equinox JAR decompile inventory

Tool: **CFR 0.152** (`brew install cfr-decompiler`). 100% success rate, no fallback to `javap` was needed. JDK: Temurin 21.0.10. Outputs live in each demo's `decompiled/` subdir.

All applets are by **Keops / Equinox** (`http://equinox.planet-d.net/`); palette-indexed framebuffers blitted via `MemoryImageSource` + `IndexColorModel(8, 256, ...)`. Tag string `"THE EQUINOX TOUCH"` appears in most.

| Demo dir | JAR | Year | Classes (main + helpers) | Status | Framebuffer | Notes / strings |
|---|---|---|---|---|---|---|
| blurredlines | lineblur.jar | 2000-09 | `lineblur`, `a` | ok | 250x250 (`new byte[62500]`, IndexColorModel) | three 62500-byte planes (front/back/blur accumulator) |
| fractals | Quaternions.jar | 2001-11 | `Quaternions`, `a`, `b` | ok | dynamic `n*n*3` 24-bit | quaternion julia; uses Color(77,92,189) accent + black bg |
| ltp4 | Ltp.jar | 2004-01 | `Troisd` (main, ~37 KB), 24 obfuscated helpers under `a/`, `b/{a..g}/`, `routs/optimizeface/` | ok | dynamic `H x I` (param-driven), supports 8-bit indexed and 24-bit direct | uses string-decryption (`Troisd.c("...")`); param `eqx` parsed; loads remote URL resources |
| megadist | Scrolls88.jar | 2000-09 | `Scrolls88`, `a` | ok | **320x200** (`new byte[64000]` x3) | classic ST resolution; reads applet param `eqx` |
| oldschoolintro | Scrolls88.jar | 2000-09 | (byte-identical to megadist) | ok | 320x200 | duplicate JAR |
| particles | Particules.jar | 2000-05 | `Particules`, `a`-`f` | ok | **300x300** (`new byte[90000]`) | embeds 256-entry RGB palette (`ar`) and 396-byte sprite/font bitmap (`at`) inline |
| pupul | Pupul.jar | 2000-07 | `Pupul`, `a` | ok | dynamic `at x as` | string `"Pupul Atari intro (Equinox 1989)"`; loads `dragon1..8.gif`, `fondpupul2.gif`, `fondpupul22.gif`, `fonte.gif`; 12 MemoryImageSource slots, 11 sprite arrays of 800 ints, 9-frame `Image[] a_` |
| rotoscroll | RotoScroll.jar | 2000-02 | `DemoSquelette`, `a`, `b` | ok | dynamic `m x l` indexed | loads `ballred.gif`; minimalist black/white roto-zoom skeleton |
| soundchip_emulator | YmMain.jar | 2000-11 | `YmMain`, `a`-`g` | ok | n/a (audio + UI) | `"Yamaha2149 Emulator by Equinox"`; loads `LogoYM.jpg`, `eqxlogo.gif`, `ball14x14.gif`, `raster16.gif`, `fonte_e/q/x.gif` |
| unlimited3dbobs | UnlimitedBobs.jar | 2000-04 | (byte-identical to unlimitedbobs) | ok | dynamic | duplicate JAR |
| unlimitedbobs | UnlimitedBobs.jar | 2000-04 | `UnlimitedBobs`, `a` | ok | dynamic, sprite-based blit | loads `sprites2.gif`; arrays of 8 (`Image[8]`, `int[8]` x6) suggest 8 colour layers/frames; palette accent `Color(170,150,200)` |
| vectorball | Vectorball.jar | 2000-09 | `Vectorball`, `a` (22 KB, holds vector tables) | ok | **300x280-300** (`af=280` or `300`, `ag=300`) | 408-int + 413-byte rotation tables; 4x10 double matrix (transform stack); 20-int scratch |
| vectordotballs | Vectorball.jar | 2000-09 | (byte-identical to vectorball) | ok | 300x300 | duplicate JAR |
| verticalrasters | VertRasters.jar | 2000-07 | `VertRasters`, `a` | ok | **384x260** (`ax=384`, `aw=260`) | `"Vertical Rasters (Atari ST effect conversion)"`; loads `equinox.gif`, `the_ball.gif`, `fontes.gif`; two 256-entry palette LUTs |
| weirdballs | WeirdBalls.jar | 2000-03 | `WeirdBalls`, `a` (18 KB tables) | ok | **300x300** (`ad=300`, `ae=300`) | 4x10 double matrix; 408-int rotation table; 256-entry palette triplet (`l`/`m`/`n`) |

## Patterns reusable for ESP32 port

- **Palette pipeline** is uniform: `byte[256] r,g,b` arrays + 8-bit framebuffer. Maps cleanly to ESP32 RGB565 LUT lookup per pixel.
- **Vector ball / weirdballs** share a 4x10 double transform matrix and a 408-int sprite/index table -- likely the same precomputed sphere of 408 points.
- **Particles** ships its 256-entry palette and sprite mask **inline as byte[]** (signed, so values >127 wrap negative); easy to lift directly.
- **ltp4 / Troisd** uses string obfuscation `Troisd.c(String)` -- decode at runtime with that helper before reading resource paths or param names.
- **Common framebuffer sizes** to plan letterboxing for 360x360 round display: 250x250, 300x300, 320x200, 384x260.
