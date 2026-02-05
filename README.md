# MLX Raylib - Drop-in MinilibX Replacement

A modern, cross-platform implementation of the MinilibX graphics library using [Raylib](https://www.raylib.com/) as the backend. This library provides a drop-in replacement for MinilibX that works on Linux, macOS, Windows, and **WebAssembly (browsers)**.

## Features

- ✅ **Cross-platform**: Works on Linux, macOS, Windows, and web browsers
- ✅ **Drop-in replacement**: Compatible with existing MLX projects
- ✅ **WebAssembly support**: Compile your MLX projects to run in browsers
- ✅ **Modern graphics**: Uses Raylib's efficient OpenGL backend
- ✅ **No X11 dependency**: Works without X11/XQuartz

## Supported Functions

| Function | Status | Notes |
|----------|--------|-------|
| `mlx_init` | ✅ | Initializes Raylib context |
| `mlx_new_window` | ✅ | Creates a Raylib window |
| `mlx_destroy_window` | ✅ | Closes window |
| `mlx_new_image` | ✅ | Creates image buffer |
| `mlx_get_data_addr` | ✅ | Returns image data pointer |
| `mlx_put_image_to_window` | ✅ | Renders image to window |
| `mlx_pixel_put` | ✅ | Direct pixel drawing |
| `mlx_loop` | ✅ | Main event loop (WASM compatible) |
| `mlx_loop_hook` | ✅ | Register loop callback |
| `mlx_loop_end` | ✅ | Exit loop |
| `mlx_key_hook` | ✅ | Key event handler |
| `mlx_mouse_hook` | ✅ | Mouse button handler |
| `mlx_hook` | ✅ | Generic event hooks |
| `mlx_get_screen_size` | ✅ | Returns screen dimensions |
| `mlx_mouse_get_pos` | ✅ | Get mouse position |
| `mlx_mouse_move` | ✅ | Set mouse position (native only) |
| `mlx_mouse_show` | ✅ | Show cursor |
| `mlx_mouse_hide` | ✅ | Hide cursor |
| `mlx_xpm_file_to_image` | ✅ | Load XPM images |
| `mlx_destroy_image` | ✅ | Free image memory |
| `mlx_destroy_display` | ✅ | Cleanup |
| `mlx_string_put` | ⚠️ | Basic support |

## Quick Start

### Prerequisites

**For Native Build:**
- GCC or Clang
- Raylib 4.5+ (see installation below)

**For WASM/Browser Build:**
- [Emscripten SDK](https://emscripten.org/)
- Raylib compiled for WASM

### Installation

1. Clone this repository:
```bash
git clone https://github.com/zakaria-mourtaban/mlx_raylib.git
cd mlx_raylib
```

2. Install Raylib (if not already installed):
```bash
make install-deps
```

3. Build the library:
```bash
make
```

### Using in Your Project

Simply copy `mlx.h`, `mlx_int.h`, and all `.c` files to your project, or use as a library:

```bash
# Copy to your project
cp mlx*.h mlx*.c /path/to/your/project/

# Or use the compiled library
gcc -o your_program your_code.c -L/path/to/mlx_raylib -lmlx -lraylib -lm
```

## WebAssembly (Browser) Build

This library supports compiling MLX projects to run in web browsers using Emscripten.

### Setting up Emscripten

```bash
# Install Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
cd ~/emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### Building Raylib for WASM

```bash
git clone https://github.com/raysan5/raylib.git ~/raylib-wasm
cd ~/raylib-wasm/src
make PLATFORM=PLATFORM_WEB -B
```

This creates `libraylib.web.a`.

### Building MLX for WASM

```bash
cd /path/to/mlx_raylib
make wasm
```

This creates `libmlx_wasm.a`.

### Compiling Your Project for Browser

```bash
emcc -Os your_main.c \
    -I/path/to/mlx_raylib \
    -I~/raylib-wasm/src \
    /path/to/mlx_raylib/libmlx_wasm.a \
    ~/raylib-wasm/src/libraylib.web.a \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    -s TOTAL_MEMORY=67108864 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -o output.html
```

### WASM Build Flags Explained

| Flag | Purpose |
|------|---------|
| `-s USE_GLFW=3` | Use Emscripten's GLFW implementation for window/input |
| `-s ASYNCIFY` | Required for blocking operations like `mlx_loop` |
| `-s TOTAL_MEMORY=67108864` | Initial memory (64MB, adjust as needed) |
| `-s ALLOW_MEMORY_GROWTH=1` | Allow dynamic memory allocation |

### Serving WASM Files

```bash
cd output_directory
python3 -m http.server 8080
# Open http://localhost:8080/output.html in browser
```

### File Loading in WASM

For loading files (like `.rt` scene files), you need to preload them:

```bash
emcc ... --preload-file scenes/
```

This embeds the `scenes/` directory into the WASM binary.

## Architecture

```
┌─────────────────────────────────────────────────┐
│              Your MLX Application                │
├─────────────────────────────────────────────────┤
│                  mlx.h API                       │
├─────────────────────────────────────────────────┤
│            MLX Raylib Backend                    │
│  ┌─────────────────────────────────────────────┐│
│  │ mlx_init.c   - Initialization               ││
│  │ mlx_window.c - Window management            ││
│  │ mlx_image.c  - Image buffers                ││
│  │ mlx_loop.c   - Event loop (WASM compatible) ││
│  │ mlx_hook.c   - Event callbacks              ││
│  │ mlx_keys.c   - X11 keycode mapping          ││
│  │ mlx_mouse.c  - Mouse handling               ││
│  │ mlx_xpm.c    - XPM image loading            ││
│  └─────────────────────────────────────────────┘│
├─────────────────────────────────────────────────┤
│                   Raylib                         │
├─────────────────────────────────────────────────┤
│    OpenGL (native) │ WebGL (WASM/browser)        │
└─────────────────────────────────────────────────┘
```

## Key Mapping

The library translates Raylib keycodes to X11 keycodes for compatibility with existing MLX projects:

```c
// Example: ESC key
#define XK_Escape 65307

// In your code
int key_handler(int keycode, void *param)
{
    if (keycode == 65307) // ESC
        mlx_loop_end(mlx);
    return 0;
}
```

See `mlx_keys.c` for the complete keycode mapping table.

## Performance Tips

1. **Use `mlx_put_image_to_window`**: Much faster than `mlx_pixel_put` for bulk drawing
2. **Minimize window size**: Smaller windows = faster rendering
3. **WASM memory**: Set `TOTAL_MEMORY` appropriately for your project

## WASM Limitations

- `mlx_mouse_move()` - Cannot programmatically move cursor in browsers
- File system access requires preloading files with `--preload-file`
- Some key combinations may be intercepted by the browser

## Differences from Original MinilibX

1. **No X11 dependency**: Uses Raylib's cross-platform windowing
2. **Endianness**: Image data is always in little-endian BGRA format
3. **Loop behavior**: Uses `emscripten_set_main_loop` for WASM builds
4. **Thread safety**: Not guaranteed, use single-threaded design

## Testing

```bash
# Build and run test
make test
./test_mlx

# Run WASM test
cd test
python3 -m http.server 8080
# Open http://localhost:8080/wasm_test.html
```

## Troubleshooting

### "raylib.h not found"
- Set `RAYLIB_PATH` when building: `make RAYLIB_PATH=/path/to/raylib`

### WASM not loading
- Ensure you're serving via HTTP (not file://)
- Check browser console for errors
- Verify all files (.html, .js, .wasm) are accessible

### Black screen in browser
- Add `-s ASYNCIFY` flag to emcc
- Ensure `mlx_loop` is called

### Keyboard not working in WASM
- Click the canvas first to focus it
- Some keys may be blocked by browser shortcuts

## License

MIT License - see LICENSE file

## Credits

- [Raylib](https://www.raylib.com/) - A simple and easy-to-use library to enjoy videogames programming
- [MinilibX](https://github.com/42Paris/minilibx-linux) - Original X11 graphics library
- [Emscripten](https://emscripten.org/) - LLVM to WebAssembly compiler

## Contributing

Pull requests welcome! Please test on both native and WASM platforms.
