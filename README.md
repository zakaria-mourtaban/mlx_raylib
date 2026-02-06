# MLX Raylib — MinilibX for Desktop & Web

A **drop-in replacement** for [MinilibX](https://github.com/42Paris/minilibx-linux) built on top of [Raylib](https://www.raylib.com/). Write your 42 project once, run it natively **and** in the browser via WebAssembly.

![Native + WASM](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20WASM-blue)
![License](https://img.shields.io/badge/license-MIT-green)

---

## ✨ Features

| | Native | WASM (Browser) |
|---|---|---|
| Window management | ✅ | ✅ |
| Image rendering | ✅ | ✅ |
| Keyboard input (X11 keycodes) | ✅ | ✅ |
| Mouse input & motion | ✅ | ✅ |
| XPM image loading | ✅ | ✅ |
| Pixel put / string put | ✅ | ✅ |
| JS ↔ C bridge (optional) | — | ✅ |

**API-compatible** with MinilibX — existing `mlx_*` function calls work without changes.

---

## 🚀 Quick Start

### 1. Clone & Setup

```bash
git clone https://github.com/zakaria-mourtaban/mlx_raylib.git
cd mlx_raylib
./setup.sh
```

The setup script will:
- Install Raylib (native) if not present
- Install Emscripten SDK if not present
- Build Raylib for WASM
- Build `libmlx.a` and `libmlx_wasm.a`

> **Skip WASM?** Run `./setup.sh --native-only`
> **Skip native?** Run `./setup.sh --wasm-only`

### 2. Use in Your Project

Copy `Makefile.template` to your project root:

```bash
cp mlx_raylib/Makefile.template myproject/Makefile
```

Edit the top section:

```makefile
NAME        = my_app
SRCS        = main.c render.c utils.c
MLX_DIR     = path/to/mlx_raylib
```

Build and run:

```bash
make              # Native binary
make wasm          # Build for browser
make serve         # Build + start local server at http://localhost:8080
```

---

## 📁 Project Structure

```
mlx_raylib/
├── mlx.h                  # Public API header (include this)
├── mlx_wasm_bridge.h      # Optional WASM ↔ JS bridge header
├── mlx_int.h              # Internal structures
├── mlx_*.c                # Implementation files
├── Makefile               # Library build (libmlx.a / libmlx_wasm.a)
├── Makefile.template       # Copy this to your project
├── setup.sh               # One-command environment setup
├── shell_default.html     # Default HTML template for WASM builds
├── examples/
│   ├── basic/             # Minimal example (gradient animation)
│   └── bridge/            # JS ↔ C bridge demo
└── README.md
```

---

## 🔧 Building the Library

If you prefer manual builds:

```bash
# Native
make

# WASM (requires emsdk in PATH)
source ~/emsdk/emsdk_env.sh
make wasm

# WASM with JS bridge support
make wasm-bridge
```

---

## �� WASM Build Guide

### Prerequisites

1. **Emscripten SDK** — installed by `setup.sh` or manually:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
   cd ~/emsdk && ./emsdk install latest && ./emsdk activate latest
   source ~/emsdk/emsdk_env.sh
   ```

2. **Raylib built for WASM** — installed by `setup.sh` or manually:
   ```bash
   git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git ~/raylib-wasm
   cd ~/raylib-wasm/src
   make PLATFORM=PLATFORM_WEB -j$(nproc)
   mv libraylib.a libraylib.web.a
   ```

---

## ⚠️ Compiling Your Own Libraries for WASM

**This is the most common issue when building 42 projects for WASM.**

If your project uses `libft.a`, `libftprintf.a`, or any other static libraries, you **MUST** recompile them with `emcc` (the Emscripten compiler). Native `.o` files compiled with `gcc` or `clang` are **not** compatible with WASM.

### The Problem

```
wasm-ld: warning: ./libft/libft.a: archive member 'ft_strlen.o' is neither Wasm object file nor LLVM bitcode
wasm-ld: error: undefined symbol: ft_strlen
```

This means `libft.a` was compiled with `gcc`, producing x86 object files. WASM needs WebAssembly object files.

### The Solution

Add a `wasm` target to each library's Makefile:

**libft/Makefile:**
```makefile
NAME = libft.a
WASM_NAME = libft_wasm.a

CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRCS = ft_strlen.c ft_strcpy.c ft_atoi.c ...
OBJS = $(SRCS:.c=.o)
WASM_OBJS = $(SRCS:.c=.wasm.o)

all: $(NAME)

$(NAME): $(OBJS)
ar rcs $(NAME) $(OBJS)

# WASM target - compile with emcc, archive with emar
wasm: $(WASM_NAME)

$(WASM_NAME): $(WASM_OBJS)
emar rcs $(WASM_NAME) $(WASM_OBJS)

%.o: %.c
$(CC) $(CFLAGS) -c $< -o $@

%.wasm.o: %.c
emcc -Wall -Wextra -O3 -c $< -o $@

clean:
rm -f $(OBJS) $(WASM_OBJS)

fclean: clean
rm -f $(NAME) $(WASM_NAME)
```

**Key differences for WASM:**
- Use `emcc` instead of `gcc` for compilation
- Use `emar` instead of `ar` for archiving
- Name the output `libft_wasm.a` to distinguish from native

### Project Makefile Example

```makefile
NAME = miniRT
SRCS = main.c render.c parse.c ...

# Library directories
LIBFT_DIR = ./libft
PRINTF_DIR = ./ft_printf
MLX_DIR = ./mlx_raylib

# Native build
all: $(NAME)

$(NAME): $(SRCS)
make -C $(LIBFT_DIR)
make -C $(PRINTF_DIR)
make -C $(MLX_DIR)
gcc -o $(NAME) $(SRCS) \
-L$(LIBFT_DIR) -L$(PRINTF_DIR) -L$(MLX_DIR) \
-lft -lftprintf -lmlx \
-lraylib -lGL -lm -lpthread -ldl -lrt

# WASM build - build WASM versions of ALL libraries first
RAYLIB_WASM = ~/raylib-wasm/src/libraylib.web.a

wasm:
make -C $(LIBFT_DIR) wasm        # Build libft_wasm.a
make -C $(PRINTF_DIR) wasm       # Build libftprintf_wasm.a
make -C $(MLX_DIR) wasm          # Build libmlx_wasm.a
emcc -O3 -o $(NAME).html $(SRCS) \
-I$(MLX_DIR) \
-L$(LIBFT_DIR) -L$(PRINTF_DIR) -L$(MLX_DIR) \
-lft_wasm -lftprintf_wasm -lmlx_wasm \
$(RAYLIB_WASM) \
-sUSE_GLFW=3 -sASYNCIFY -sTOTAL_MEMORY=67108864 \
--shell-file $(MLX_DIR)/shell_default.html

clean:
make -C $(LIBFT_DIR) clean
make -C $(PRINTF_DIR) clean
make -C $(MLX_DIR) clean
rm -f $(NAME) $(NAME).html $(NAME).js $(NAME).wasm
```

### Quick Reference

| Tool | Native | WASM |
|------|--------|------|
| Compiler | `gcc` / `clang` | `emcc` |
| Archiver | `ar` | `emar` |
| Linker | `ld` / `gcc` | `emcc` |
| Output | `.o` (ELF/Mach-O) | `.wasm.o` (WASM) |

### Checklist for WASM Builds

- [ ] Source emsdk: `source ~/emsdk/emsdk_env.sh`
- [ ] Add `wasm` target to `libft/Makefile`
- [ ] Add `wasm` target to `ft_printf/Makefile` (if using)
- [ ] Add `wasm` target to any other library Makefiles
- [ ] In main Makefile, call `make wasm` for each library before linking
- [ ] Link with `-l<name>_wasm` instead of `-l<name>`
- [ ] Include the raylib WASM library in the link

---

## 🌉 WASM Bridge (Optional)

The bridge lets JavaScript and your C code communicate at runtime — restart with different arguments, send commands, receive status updates.

### Enable the Bridge

1. Include the header in your C code:
   ```c
   #include "mlx_wasm_bridge.h"
   ```

2. Build with bridge support:
   ```makefile
   WASM_BRIDGE = yes   # In your Makefile
   ```
   Or: `make -C mlx_raylib wasm-bridge`

### C Side — Checking for Commands

Poll in your `loop_hook`:

```c
int render(void *param)
{
    t_app *app = (t_app *)param;

    // Check if JS requested a restart
    if (mlx_bridge_restart_requested())
    {
        char **argv;
        int argc = mlx_bridge_get_args(&argv);
        reload_scene(app, argv[1]);
    }

    // Check if JS sent a command
    char *cmd = mlx_bridge_get_command();
    if (cmd)
    {
        if (strcmp(cmd, "toggle_debug") == 0)
            app->debug = !app->debug;
    }

    // Send status back to JS
    mlx_bridge_send_status("Rendering frame 42");

    return (0);
}
```

### JavaScript Side

```javascript
// Send a command
var ptr = Module.allocateUTF8("toggle_debug");
Module._mlx_bridge_send_command(ptr);
Module._free(ptr);

// Restart with new arguments
Module._mlx_bridge_clear_args();
Module._mlx_bridge_add_arg(Module.allocateUTF8("app"));
Module._mlx_bridge_add_arg(Module.allocateUTF8("scenes/new.rt"));
Module._mlx_bridge_restart();
```

---

## ⌨️ Key Codes

MLX Raylib outputs **X11 keycodes** (same as MinilibX on Linux), regardless of platform:

| Key | Code | Key | Code |
|---|---|---|---|
| ESC | 65307 | Space | 32 |
| W | 119 | A | 97 |
| S | 115 | D | 100 |
| ↑ | 65362 | ↓ | 65364 |
| ← | 65361 | → | 65363 |

> **Note for 42 projects:** If your code uses `#ifdef __linux__` for keycodes, add `|| defined(__EMSCRIPTEN__)` to use the Linux keycodes in WASM builds too.

---

## 🔀 Migrating from MinilibX

1. Replace your `minilibx/` directory with `mlx_raylib/`
2. Update include path: `-Imlx_raylib` instead of `-Iminilibx`
3. Link: `mlx_raylib/libmlx.a` + `-lraylib -lGL -lm -lpthread -ldl -lrt`
4. Remove `-lXext -lX11` (no longer needed)
5. Build and test — everything should work identically

For WASM, add a `wasm` target to your Makefile (see above).

---

## 🐛 Troubleshooting

### "archive member is neither Wasm object file nor LLVM bitcode"
Your library was compiled with gcc/clang. Recompile it with `emcc` (see above).

### Black screen in WASM
- Make sure you call `mlx_loop()` — it sets up Emscripten's main loop
- Check browser console for errors

### Keyboard not working in WASM
- Click on the canvas first (browser requires focus)
- If using `#ifdef __linux__` for keycodes, add `|| defined(__EMSCRIPTEN__)`

### Build errors with emcc
- Run `source ~/emsdk/emsdk_env.sh` before building
- Make sure Raylib WASM library path is correct

### Large WASM file size
- Use `-Os` optimization flag
- Only `--preload-file` the assets you need
- The base binary is ~150KB WASM + ~180KB JS

---

## 📄 License

MIT License — see [LICENSE](LICENSE).

Built with [Raylib](https://www.raylib.com/) by [raysan5](https://github.com/raysan5).
