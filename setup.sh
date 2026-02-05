#!/bin/bash
# **************************************************************************** #
#    MLX Raylib - Setup Script                                                 #
#                                                                              #
#    This script installs everything needed to build MLX projects for          #
#    both native desktop and WASM/web.                                         #
#                                                                              #
#    Usage: ./setup.sh [--native-only] [--wasm-only]                           #
# **************************************************************************** #

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

EMSDK_PATH="${EMSDK_PATH:-$HOME/emsdk}"
RAYLIB_WASM_PATH="${RAYLIB_WASM_PATH:-$HOME/raylib-wasm}"
RAYLIB_VERSION="5.5"

info()  { echo -e "${BLUE}→ $1${NC}"; }
ok()    { echo -e "${GREEN}✓ $1${NC}"; }
warn()  { echo -e "${YELLOW}⚠ $1${NC}"; }
fail()  { echo -e "${RED}✗ $1${NC}"; exit 1; }

# ─── Parse arguments ─────────────────────────────────────────────────────── #

NATIVE=true
WASM=true

for arg in "$@"; do
    case $arg in
        --native-only) WASM=false ;;
        --wasm-only)   NATIVE=false ;;
        --help|-h)
            echo "Usage: $0 [--native-only] [--wasm-only]"
            echo ""
            echo "  --native-only   Only install native dependencies (raylib)"
            echo "  --wasm-only     Only install WASM dependencies (emsdk + raylib wasm)"
            echo ""
            echo "Environment variables:"
            echo "  EMSDK_PATH        Emscripten SDK location (default: ~/emsdk)"
            echo "  RAYLIB_WASM_PATH  Raylib WASM build location (default: ~/raylib-wasm)"
            exit 0
            ;;
    esac
done

echo ""
echo "╔══════════════════════════════════════════════════════════╗"
echo "║          MLX Raylib — Environment Setup                 ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

# ─── Detect OS ───────────────────────────────────────────────────────────── #

OS="$(uname -s)"
info "Detected OS: $OS"

# ─── Native Raylib ───────────────────────────────────────────────────────── #

if [ "$NATIVE" = true ]; then
    echo ""
    echo "── Native Raylib ──────────────────────────────────────────"

    if pkg-config --exists raylib 2>/dev/null || [ -f /usr/local/lib/libraylib.a ]; then
        ok "Raylib already installed"
    else
        info "Installing raylib build dependencies..."
        if [ "$OS" = "Linux" ]; then
            if command -v apt-get &>/dev/null; then
                sudo apt-get update -qq
                sudo apt-get install -y -qq build-essential git cmake \
                    libx11-dev libxrandr-dev libxi-dev libxcursor-dev \
                    libxinerama-dev libgl1-mesa-dev 2>/dev/null
            elif command -v dnf &>/dev/null; then
                sudo dnf install -y gcc make git cmake \
                    libX11-devel libXrandr-devel libXi-devel libXcursor-devel \
                    libXinerama-devel mesa-libGL-devel 2>/dev/null
            elif command -v pacman &>/dev/null; then
                sudo pacman -S --noconfirm --needed base-devel git cmake \
                    libx11 libxrandr libxi libxcursor libxinerama mesa 2>/dev/null
            else
                warn "Unknown package manager. Please install raylib manually."
                warn "See: https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux"
            fi
        elif [ "$OS" = "Darwin" ]; then
            if ! command -v brew &>/dev/null; then
                warn "Homebrew not found. Install from https://brew.sh"
            fi
        fi

        info "Building raylib ${RAYLIB_VERSION} from source..."
        TMPDIR=$(mktemp -d)
        cd "$TMPDIR"
        git clone --depth 1 --branch "${RAYLIB_VERSION}" \
            https://github.com/raysan5/raylib.git 2>/dev/null
        cd raylib/src
        make PLATFORM=PLATFORM_DESKTOP -j$(nproc 2>/dev/null || echo 4) 2>/dev/null
        sudo make install 2>/dev/null
        cd /
        rm -rf "$TMPDIR"
        ok "Raylib ${RAYLIB_VERSION} installed to /usr/local"
    fi
fi

# ─── Emscripten SDK ──────────────────────────────────────────────────────── #

if [ "$WASM" = true ]; then
    echo ""
    echo "── Emscripten SDK ─────────────────────────────────────────"

    if [ -d "$EMSDK_PATH" ] && [ -f "$EMSDK_PATH/emsdk_env.sh" ]; then
        ok "Emscripten SDK found at $EMSDK_PATH"
    else
        info "Installing Emscripten SDK to $EMSDK_PATH..."
        git clone https://github.com/emscripten-core/emsdk.git "$EMSDK_PATH" 2>/dev/null
        cd "$EMSDK_PATH"
        ./emsdk install latest 2>/dev/null
        ./emsdk activate latest 2>/dev/null
        ok "Emscripten SDK installed"
    fi

    # Activate emsdk for this session
    source "$EMSDK_PATH/emsdk_env.sh" 2>/dev/null

    # Verify emcc works
    if command -v emcc &>/dev/null; then
        EMCC_VER=$(emcc --version | head -n1 | grep -oP '\d+\.\d+\.\d+' || echo "unknown")
        ok "emcc version: $EMCC_VER"
    else
        fail "emcc not found after SDK activation"
    fi

    # ─── Raylib WASM build ────────────────────────────────────────────────── #

    echo ""
    echo "── Raylib WASM Build ──────────────────────────────────────"

    if [ -f "$RAYLIB_WASM_PATH/src/libraylib.web.a" ]; then
        ok "Raylib WASM library found at $RAYLIB_WASM_PATH"
    else
        info "Building raylib for WASM..."
        if [ ! -d "$RAYLIB_WASM_PATH" ]; then
            git clone --depth 1 --branch "${RAYLIB_VERSION}" \
                https://github.com/raysan5/raylib.git "$RAYLIB_WASM_PATH" 2>/dev/null
        fi
        cd "$RAYLIB_WASM_PATH/src"
        make clean 2>/dev/null || true
        make PLATFORM=PLATFORM_WEB -j$(nproc 2>/dev/null || echo 4) 2>/dev/null
        if [ -f libraylib.a ]; then
            mv libraylib.a libraylib.web.a
        fi
        ok "Raylib WASM library built"
    fi
fi

# ─── Build MLX library ───────────────────────────────────────────────────── #

echo ""
echo "── Building MLX Raylib ────────────────────────────────────"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ "$NATIVE" = true ]; then
    make fclean 2>/dev/null || true
    make all 2>/dev/null
    ok "libmlx.a built"
fi

if [ "$WASM" = true ]; then
    make wasm RAYLIB_WASM_PATH="$RAYLIB_WASM_PATH" 2>/dev/null
    ok "libmlx_wasm.a built"
fi

# ─── Summary ─────────────────────────────────────────────────────────────── #

echo ""
echo "╔══════════════════════════════════════════════════════════╗"
echo "║                    Setup Complete!                      ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""

if [ "$NATIVE" = true ]; then
    echo "  Native build:"
    echo "    Raylib:    $(pkg-config --modversion raylib 2>/dev/null || echo '/usr/local')"
    echo "    Library:   $SCRIPT_DIR/libmlx.a"
    echo ""
fi

if [ "$WASM" = true ]; then
    echo "  WASM build:"
    echo "    Emscripten: $EMSDK_PATH"
    echo "    Raylib:     $RAYLIB_WASM_PATH/src/libraylib.web.a"
    echo "    Library:    $SCRIPT_DIR/libmlx_wasm.a"
    echo ""
    echo "  Add to your shell profile:"
    echo "    source $EMSDK_PATH/emsdk_env.sh"
    echo ""
fi

echo "  Quick start:"
echo "    1. Copy Makefile.template to your project as Makefile"
echo "    2. Set MLX_DIR to point to this directory"
echo "    3. Run 'make' for native or 'make wasm' for web"
echo ""
echo "  See README.md for full documentation."
echo ""
