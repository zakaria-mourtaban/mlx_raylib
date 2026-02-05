# MinilibX Raylib Backend Makefile
# Supports native compilation and WASM build

NAME = libmlx.a
NAME_WASM = libmlx_wasm.a

CC = gcc
CC_WASM = emcc

CFLAGS = -Wall -Wextra -Werror -O3 -std=gnu99
CFLAGS_WASM = -Wall -Wextra -O3 -std=gnu99

# Raylib paths (adjust as needed)
RAYLIB_PATH ?= /usr/local
RAYLIB_INCLUDE = -I$(RAYLIB_PATH)/include
RAYLIB_LIB = -L$(RAYLIB_PATH)/lib -lraylib

# For WASM build
EMSDK_PATH ?= $(HOME)/emsdk
RAYLIB_WASM_PATH ?= $(HOME)/raylib-wasm

SRCS = mlx_init.c \
       mlx_window.c \
       mlx_image.c \
       mlx_hook.c \
       mlx_loop.c \
       mlx_mouse.c \
       mlx_pixel_put.c \
       mlx_utils.c \
       mlx_keys.c \
       mlx_xpm.c

OBJS = $(SRCS:.c=.o)
OBJS_WASM = $(SRCS:.c=.wasm.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)
	@echo "Built $(NAME)"

%.o: %.c mlx.h mlx_int.h
	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -c $< -o $@

# WASM build target
wasm: $(NAME_WASM)

$(NAME_WASM): $(OBJS_WASM)
	emar rcs $(NAME_WASM) $(OBJS_WASM)
	@echo "Built $(NAME_WASM)"

%.wasm.o: %.c mlx.h mlx_int.h
	$(CC_WASM) $(CFLAGS_WASM) -I$(RAYLIB_WASM_PATH)/src -c $< -o $@

clean:
	rm -f $(OBJS) $(OBJS_WASM)

fclean: clean
	rm -f $(NAME) $(NAME_WASM)

re: fclean all

# Install raylib dependencies
install-deps:
	@echo "Installing raylib..."
	@if [ ! -d "/tmp/raylib" ]; then \
		git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib; \
	fi
	@cd /tmp/raylib/src && make PLATFORM=PLATFORM_DESKTOP
	@sudo cp /tmp/raylib/src/libraylib.a /usr/local/lib/
	@sudo cp /tmp/raylib/src/raylib.h /usr/local/include/
	@sudo cp /tmp/raylib/src/raymath.h /usr/local/include/
	@sudo cp /tmp/raylib/src/rlgl.h /usr/local/include/
	@echo "Raylib installed to /usr/local"

# Test build
test: $(NAME)
	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -o test_mlx test/main.c \
		-L. -lmlx $(RAYLIB_LIB) -lm -lpthread -ldl

.PHONY: all clean fclean re wasm install-deps test
