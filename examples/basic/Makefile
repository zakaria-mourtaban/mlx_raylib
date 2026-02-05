# **************************************************************************** #
#    Basic MLX Example                                                        #
# **************************************************************************** #

NAME		= basic_demo

SRCS		= main.c
MLX_DIR		= ../../

RAYLIB_PATH		?= /usr/local
RAYLIB_WASM_PATH	?= $(HOME)/raylib-wasm

CC		= cc
CC_WASM		= emcc
CFLAGS		= -Wall -Wextra -Werror -O2
CFLAGS_WASM	= -Wall -Wextra -Os -DPLATFORM_WEB

MLX_LIB		= $(MLX_DIR)/libmlx.a
MLX_LIB_WASM	= $(MLX_DIR)/libmlx_wasm.a
RAYLIB_WASM_LIB	= $(RAYLIB_WASM_PATH)/src/libraylib.web.a

INC		= -I$(MLX_DIR) -I$(RAYLIB_PATH)/include
INC_WASM	= -I$(MLX_DIR) -I$(RAYLIB_WASM_PATH)/src

LDFLAGS		= -L$(RAYLIB_PATH)/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

WASM_FLAGS	= -sUSE_GLFW=3 -sASYNCIFY -sTOTAL_MEMORY=67108864 -sALLOW_MEMORY_GROWTH=1
WASM_DIR	= wasm_build

all: $(MLX_LIB) $(NAME)

$(NAME): main.o $(MLX_LIB)
	$(CC) $(CFLAGS) main.o $(MLX_LIB) $(LDFLAGS) -o $(NAME)
	@echo "\033[32m✓ Built $(NAME)\033[0m"

main.o: main.c
	$(CC) $(CFLAGS) $(INC) -c main.c -o main.o

$(MLX_LIB):
	@$(MAKE) -C $(MLX_DIR) all

wasm: $(MLX_LIB_WASM) $(RAYLIB_WASM_LIB)
	@mkdir -p $(WASM_DIR)
	$(CC_WASM) $(CFLAGS_WASM) $(INC_WASM) main.c \
		$(MLX_LIB_WASM) $(RAYLIB_WASM_LIB) \
		$(WASM_FLAGS) -o $(WASM_DIR)/index.html
	@echo "\033[32m✓ WASM build in $(WASM_DIR)/\033[0m"

$(MLX_LIB_WASM):
	@$(MAKE) -C $(MLX_DIR) wasm RAYLIB_WASM_PATH=$(RAYLIB_WASM_PATH)

serve: wasm
	@echo "\033[32m→ http://localhost:8080\033[0m"
	cd $(WASM_DIR) && python3 -m http.server 8080

clean:
	rm -f main.o
	rm -rf $(WASM_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re wasm serve
