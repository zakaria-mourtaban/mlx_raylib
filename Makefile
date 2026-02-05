# **************************************************************************** #
#    MinilibX Raylib Backend - Library Makefile                                #
#    Builds libmlx.a (native) and libmlx_wasm.a (WASM)                        #
# **************************************************************************** #

NAME		= libmlx.a
NAME_WASM	= libmlx_wasm.a

CC			= cc
CC_WASM		= emcc

AR			= ar rcs
AR_WASM		= emar rcs

CFLAGS		= -Wall -Wextra -Werror -O3
CFLAGS_WASM	= -Wall -Wextra -O3 -DPLATFORM_WEB

# Raylib paths — override with: make RAYLIB_PATH=/your/path
RAYLIB_PATH		?= /usr/local
RAYLIB_INCLUDE	= -I$(RAYLIB_PATH)/include

# For WASM builds — override with: make RAYLIB_WASM_PATH=/your/path
RAYLIB_WASM_PATH	?= $(HOME)/raylib-wasm
RAYLIB_WASM_INCLUDE	= -I$(RAYLIB_WASM_PATH)/src

# --------------------------------------------------------------------------- #
#  Sources                                                                     #
# --------------------------------------------------------------------------- #

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

# Bridge is optional — compiled separately
BRIDGE_SRCS = mlx_wasm_bridge.c

OBJS		= $(SRCS:.c=.o)
OBJS_WASM	= $(addprefix obj_wasm/, $(SRCS:.c=.o))
BRIDGE_OBJ	= $(BRIDGE_SRCS:.c=.wasm.o)

# --------------------------------------------------------------------------- #
#  Native build                                                                #
# --------------------------------------------------------------------------- #

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)
	@echo "Built $(NAME)"

%.o: %.c mlx.h mlx_int.h
	$(CC) $(CFLAGS) $(RAYLIB_INCLUDE) -c $< -o $@

# --------------------------------------------------------------------------- #
#  WASM build                                                                  #
# --------------------------------------------------------------------------- #

wasm: obj_wasm $(NAME_WASM)

$(NAME_WASM): $(OBJS_WASM)
	$(AR_WASM) $(NAME_WASM) $(OBJS_WASM)
	@echo "Built $(NAME_WASM)"

obj_wasm:
	mkdir -p obj_wasm

obj_wasm/%.o: %.c mlx.h mlx_int.h
	$(CC_WASM) $(CFLAGS_WASM) $(RAYLIB_WASM_INCLUDE) -c $< -o $@

# WASM build with bridge
wasm-bridge: obj_wasm $(NAME_WASM) $(BRIDGE_OBJ)
	$(AR_WASM) $(NAME_WASM) $(OBJS_WASM) $(BRIDGE_OBJ)
	@echo "Built $(NAME_WASM) with bridge"

$(BRIDGE_OBJ): $(BRIDGE_SRCS) mlx_wasm_bridge.h
	$(CC_WASM) $(CFLAGS_WASM) $(RAYLIB_WASM_INCLUDE) -c $< -o $@

# --------------------------------------------------------------------------- #
#  Clean                                                                       #
# --------------------------------------------------------------------------- #

clean:
	rm -f $(OBJS)
	rm -rf obj_wasm
	rm -f $(BRIDGE_OBJ)

fclean: clean
	rm -f $(NAME) $(NAME_WASM)

re: fclean all

.PHONY: all clean fclean re wasm wasm-bridge
