# **************************************************************************** ## MinilibX Raylib Backend Makefile

#    MinilibX Raylib Backend - Library Makefile                                ## Supports native compilation and WASM build

#    Builds libmlx.a (native) and libmlx_wasm.a (WASM)                        #

# **************************************************************************** #NAME = libmlx.a

NAME_WASM = libmlx_wasm.a

NAME		= libmlx.a

NAME_WASM	= libmlx_wasm.aCC = gcc

CC_WASM = emcc

CC			= cc

CC_WASM		= emccCFLAGS = -Wall -Wextra -Werror -O3 -std=gnu99

AR			= ar rcsCFLAGS_WASM = -Wall -Wextra -O3 -std=gnu99

AR_WASM		= emar rcs

# Raylib paths (adjust as needed)

CFLAGS		= -Wall -Wextra -Werror -O3RAYLIB_PATH ?= /usr/local

CFLAGS_WASM	= -Wall -Wextra -O3 -DPLATFORM_WEBRAYLIB_INCLUDE = -I$(RAYLIB_PATH)/include

RAYLIB_LIB = -L$(RAYLIB_PATH)/lib -lraylib

# Raylib paths — override with: make RAYLIB_PATH=/your/path

RAYLIB_PATH		?= /usr/local# For WASM build

RAYLIB_INCLUDE	= -I$(RAYLIB_PATH)/includeEMSDK_PATH ?= $(HOME)/emsdk

RAYLIB_WASM_PATH ?= $(HOME)/raylib-wasm

# For WASM builds — override with: make RAYLIB_WASM_PATH=/your/path

RAYLIB_WASM_PATH	?= $(HOME)/raylib-wasmSRCS = mlx_init.c \

RAYLIB_WASM_INCLUDE	= -I$(RAYLIB_WASM_PATH)/src       mlx_window.c \

       mlx_image.c \

# --------------------------------------------------------------------------- #       mlx_hook.c \

#  Sources                                                                     #       mlx_loop.c \

# --------------------------------------------------------------------------- #       mlx_mouse.c \

       mlx_pixel_put.c \

SRCS = mlx_init.c \       mlx_utils.c \

       mlx_window.c \       mlx_keys.c \

       mlx_image.c \       mlx_xpm.c

       mlx_hook.c \

       mlx_loop.c \OBJS = $(SRCS:.c=.o)

       mlx_mouse.c \OBJS_WASM = $(SRCS:.c=.wasm.o)

       mlx_pixel_put.c \

       mlx_utils.c \all: $(NAME)

       mlx_keys.c \

       mlx_xpm.c$(NAME): $(OBJS)

	ar rcs $(NAME) $(OBJS)

# Bridge is optional — compiled separately	@echo "Built $(NAME)"

BRIDGE_SRC = mlx_wasm_bridge.c

%.o: %.c mlx.h mlx_int.h

OBJS		= $(SRCS:.c=.o)	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -c $< -o $@

OBJS_WASM	= $(addprefix obj_wasm/, $(SRCS:.c=.o))

BRIDGE_OBJ_WASM	= obj_wasm/mlx_wasm_bridge.o# WASM build target

wasm: $(NAME_WASM)

# --------------------------------------------------------------------------- #

#  Rules                                                                       #$(NAME_WASM): $(OBJS_WASM)

# --------------------------------------------------------------------------- #	emar rcs $(NAME_WASM) $(OBJS_WASM)

	@echo "Built $(NAME_WASM)"

all: $(NAME)

%.wasm.o: %.c mlx.h mlx_int.h

$(NAME): $(OBJS)	$(CC_WASM) $(CFLAGS_WASM) -I$(RAYLIB_WASM_PATH)/src -c $< -o $@

	$(AR) $(NAME) $(OBJS)

	@echo "\033[32m✓ Built $(NAME)\033[0m"clean:

	rm -f $(OBJS) $(OBJS_WASM)

%.o: %.c mlx.h mlx_int.h

	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -c $< -o $@fclean: clean

	rm -f $(NAME) $(NAME_WASM)

# --- WASM targets --- #

re: fclean all

wasm: obj_wasm $(NAME_WASM)

	@echo "\033[32m✓ Built $(NAME_WASM)\033[0m"# Install raylib dependencies

install-deps:

# WASM with bridge support	@echo "Installing raylib..."

wasm-bridge: obj_wasm $(OBJS_WASM) $(BRIDGE_OBJ_WASM)	@if [ ! -d "/tmp/raylib" ]; then \

	$(AR_WASM) $(NAME_WASM) $(OBJS_WASM) $(BRIDGE_OBJ_WASM)		git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib; \

	@echo "\033[32m✓ Built $(NAME_WASM) (with bridge)\033[0m"	fi

	@cd /tmp/raylib/src && make PLATFORM=PLATFORM_DESKTOP

$(NAME_WASM): $(OBJS_WASM)	@sudo cp /tmp/raylib/src/libraylib.a /usr/local/lib/

	$(AR_WASM) $(NAME_WASM) $(OBJS_WASM)	@sudo cp /tmp/raylib/src/raylib.h /usr/local/include/

	@sudo cp /tmp/raylib/src/raymath.h /usr/local/include/

obj_wasm:	@sudo cp /tmp/raylib/src/rlgl.h /usr/local/include/

	@mkdir -p obj_wasm	@echo "Raylib installed to /usr/local"



obj_wasm/%.o: %.c mlx.h mlx_int.h# Test build

	$(CC_WASM) $(CFLAGS_WASM) $(RAYLIB_WASM_INCLUDE) -c $< -o $@test: $(NAME)

	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -o test_mlx test/main.c \

# --------------------------------------------------------------------------- #		-L. -lmlx $(RAYLIB_LIB) -lm -lpthread -ldl

#  Clean                                                                       #

# --------------------------------------------------------------------------- #.PHONY: all clean fclean re wasm install-deps test


clean:
	rm -f $(OBJS)
	rm -rf obj_wasm

fclean: clean
	rm -f $(NAME) $(NAME_WASM)

re: fclean all

re-wasm: fclean wasm

.PHONY: all clean fclean re wasm wasm-bridge re-wasm
