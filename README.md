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

## 🌐 WASM Build Guide

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

### Building Your Project for WASM

Using the template Makefile:

```bash
source ~/emsdk/emsdk_env.sh
make wasm
```

Or manually:

```bash
emcc -Os -DPLATFORM_WEB \
    your_sources.c \
    -Imlx_raylib \
    mlx_raylib/libmlx_wasm.a \
    ~/raylib-wasm/src/libraylib.web.a \
    -sUSE_GLFW=3 -sASYNCIFY \
    -sTOTAL_MEMORY=134217728 \
    -sALLOW_MEMORY_GROWTH=1 \
    --preload-file assets \
    --shell-file mlx_raylib/shell_default.html \
    -o wasm_build/index.html
```

### Custom HTML Shell

Copy and modify `shell_default.html`:

```bash
cp mlx_raylib/shell_default.html my_shell.html
# Edit my_shell.html to customize look and feel
```

In the `<script>` section, configure:

```javascript
var MLX_CONFIG = {
    args: ['path/to/scene.rt'],     // main() arguments
    enableFilePicker: true,          // Show file upload button
    fileExtensions: '.rt,.cub',     // Accepted file types
};
```

### Bundling Asset Files

If your project reads files at runtime (e.g., `.rt` scene files):

```makefile
WASM_ASSETS = scenes    # Directory to bundle
```

Files will be accessible at their original paths in the WASM virtual filesystem.

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
        // argv[1] might be a new scene file path
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
    mlx_bridge_send_event("progress", "50%");

    // ... normal rendering ...
    return (0);
}
```

### JavaScript Side — Sending Commands

```javascript
// Send a command string
var ptr = Module.allocateUTF8("toggle_debug");
Module._mlx_bridge_send_command(ptr);
Module._free(ptr);

// Restart with new arguments
Module._mlx_bridge_clear_args();
var name = Module.allocateUTF8("app");
Module._mlx_bridge_add_arg(name);
Module._free(name);
var scene = Module.allocateUTF8("maps/new_scene.rt");
Module._mlx_bridge_add_arg(scene);
Module._free(scene);
Module._mlx_bridge_restart();

// Listen for C events
Module.onStatusUpdate = function(msg) {
    document.getElementById('status').textContent = msg;
};
Module.onBridgeEvent = function(type, data) {
    console.log('Event:', type, data);
};
```

### Bridge API Reference

| C Function | Direction | Description |
|---|---|---|
| `mlx_bridge_restart_requested()` | JS → C | Returns 1 if restart pending |
| `mlx_bridge_get_args(&argv)` | JS → C | Get new argc/argv after restart |
| `mlx_bridge_get_command()` | JS → C | Get pending command string |
| `mlx_bridge_fullscreen_requested()` | JS → C | Check fullscreen toggle |
| `mlx_bridge_pause_requested()` | JS → C | Check pause toggle |
| `mlx_bridge_send_status(msg)` | C → JS | Send status to `Module.onStatusUpdate` |
| `mlx_bridge_send_event(type, data)` | C → JS | Send event to `Module.onBridgeEvent` |

| JS Function | Direction | Description |
|---|---|---|
| `Module._mlx_bridge_send_command(ptr)` | JS → C | Send command string |
| `Module._mlx_bridge_clear_args()` | JS → C | Clear argument list |
| `Module._mlx_bridge_add_arg(ptr)` | JS → C | Add argument string |
| `Module._mlx_bridge_restart()` | JS → C | Trigger restart |
| `Module._mlx_bridge_toggle_fullscreen()` | JS → C | Toggle fullscreen |
| `Module._mlx_bridge_toggle_pause()` | JS → C | Toggle pause |

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
| 1-9 | 49-57 | L_Shift | 65505 |

> **Note for 42 projects:** If your code uses `#ifdef __linux__` for keycodes, add `|| defined(__EMSCRIPTEN__)` to use the Linux keycodes in WASM builds too.

---

## 🔀 Migrating from MinilibX

1. Replace your `minilibx/` directory with `mlx_raylib/`
2. Update include path: `-Imlx_raylib` instead of `-Iminilibx`
3. Link: `mlx_raylib/libmlx.a` + `-lraylib -lGL -lm -lpthread -ldl -lrt -lX11`
4. Remove `-lXext -lX11` (no longer needed, Raylib handles this)
5. Build and test — everything should work identically

For WASM, add the `wasm` target to your Makefile (see `Makefile.template`).

---

## 🐛 Troubleshooting

### Black screen in WASM
- Make sure you call `mlx_loop()` — it sets up Emscripten's main loop
- Check browser console for errors

### Keyboard not working in WASM
- Click on the canvas first (browser requires focus)
- If using `#ifdef __linux__` for keycodes, add `|| defined(__EMSCRIPTEN__)`

### Build errors with emcc
- Run `source ~/emsdk/emsdk_env.sh` before building
- Make sure `RAYLIB_WASM_PATH` points to a WASM-built raylib

### Large WASM file size
- Use `-Os` optimization flag
- Only `--preload-file` the assets you need
- The base binary is ~150KB WASM + ~180KB JS

---

## 📄 License

MIT License — see [LICENSE](LICENSE).

Built with [Raylib](https://www.raylib.com/) by [raysan5](https://github.com/raysan5).
