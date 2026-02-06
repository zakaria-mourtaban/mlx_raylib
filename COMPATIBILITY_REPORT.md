# mlx_raylib Compatibility Report

## Test Summary

Tested **175** MinilibX projects from GitHub (so_long, cub3D, miniRT, fdf, fractol, etc.) against the `mlx_raylib` backend.

### Results at a Glance

| Phase | Description | Pass | Fail | Skip/N/A |
|-------|------------|------|------|----------|
| **Phase 2** | Default native build (original MLX) | 36 | 120 | 19 |
| **Phase 3** | Build with mlx_raylib swapped in | 32 | 75 | 68 |
| **Phase 4** | WASM compilation (Emscripten) | 13 | 19 | — |

- **32 / 175** projects compiled successfully after swapping MinilibX → mlx_raylib (no project-side changes).
- **13 / 32** of those also compiled to WASM with Emscripten.

> Many Phase 2 failures are pre-existing build issues (missing submodules, macOS-only code, clang-specific flags). These are **not** caused by mlx_raylib.

---

## WASM-Verified Projects (Phase 4 Pass)

These 13 projects compiled to WASM out of the box:

| Repository | WASM Size |
|-----------|-----------|
| AijaRe/42Porto_FdF | 262 KB |
| JenniferAraujo/42cursus-So_long | 274 KB |
| Kuninoto/42_so_long | 282 KB |
| PedroZappa/42_fractol | 286 KB |
| RhesusP/cub3D | 284 KB |
| Sleleu/Cub3d | 281 KB |
| daniel149afonso/so_long | 283 KB |
| maciejwiacek/42solong | 284 KB |
| mcombeau/cub3D | 291 KB |
| mhmdhussein/42-cub3D | 284 KB |
| xilen0x/42_cub3D | 286 KB |
| yettabaa/FdF | 272 KB |
| yettabaa/cub3D | 277 KB |

---

## Native Build with mlx_raylib (Phase 3 Pass)

These 32 projects built natively after replacing their bundled MinilibX with mlx_raylib:

AijaRe/42Porto_FdF, GinaldoFT/so_long, JenniferAraujo/42cursus-So_long, Kuninoto/42_so_long, Nihilantropy/Cub3D, PedroZappa/42_fractol, RhesusP/cub3D, Sleleu/Cub3d, Vaizeur/so_long, alexhmball/so_long_42, dKurbi/42_09_MiniRT, damachad/42_cub3d, daniel149afonso/so_long, diocode/42-So_long, dmitrijslasko/42_fdf, extrymes/42-So-long, fbkeskin/42-cub3D, gecarval/Particle-Life, gecarval/Push-Swap-Visualizer, izenynn/fdf, josephcheel/42-So_long, maciejwiacek/42solong, mariav7/so_long_42project, mcombeau/cub3D, mhmdhussein/42-cub3D, ruipsmartins/cub3d, sgkhusal/42-miniRT, swangarch/so_long, tdnguyen98/so_long, xilen0x/42_cub3D, yettabaa/FdF, yettabaa/cub3D

---

## Common Failure Categories

The following are **project-specific** issues — they are not bugs in mlx_raylib. Users should diagnose and fix these in their own projects.

### Build Failures (Phase 3)
- **Include path mismatches**: projects hardcode `#include "../minilibx-linux/mlx.h"` or similar relative paths instead of `#include "mlx.h"`.
- **Link order / flag issues**: Makefile link lines that reference `-lmlx_Linux`, `-lmlx -lXext -lX11`, or platform-specific libraries.
- **Macro collisions**: projects that `#define PI`, `#define RED`, etc. may conflict with raylib.h if internal headers are included. Use `#ifndef` guards.
- **X11 header inclusion**: projects that explicitly `#include <X11/X.h>` or `<X11/Xlib.h>` will conflict with raylib types (e.g. `Font`). Remove direct X11 includes — mlx_raylib provides all needed defines.
- **MLX42 projects**: projects using the MLX42 library (codam-mlx) have a completely different API and are not compatible.
- **macOS-only code**: projects targeting `mlx_macos` or using macOS-specific system calls.

### WASM Failures (Phase 4)
- **Missing headers**: `aio.h`, `zlib.h`, and other POSIX/system headers not available under Emscripten.
- **Duplicate symbols**: multiple `.c` files defining the same function (often helper/utility functions).
- **Implicit function declarations**: `read()`, `write()`, `close()` — these need `#include <unistd.h>` and may need Emscripten-compatible alternatives.
- **Hardcoded relative include paths**: WASM flat compilation requires all headers to be findable via `-I` flags.

---

## How to Port Your Project

1. **Replace your MinilibX directory** with mlx_raylib (or add as submodule)
2. **Update your Makefile** to link against `-lmlx -lraylib -lGL -lm -lpthread -ldl -lrt` instead of `-lmlx -lXext -lX11`
3. **Change includes** from `#include "../minilibx-linux/mlx.h"` to `#include "mlx.h"` (and add `-I` flag to your mlx_raylib directory)
4. **Guard your macros**: if you define `PI`, `RED`, `GREEN`, etc., wrap them in `#ifndef` guards
5. **Remove direct X11 includes**: mlx_raylib provides all X11 event/mask defines you need

For WASM, see `Makefile.template` and `setup.sh` in this repo.

---

*Report generated from automated testing of 175 public GitHub repositories.*
