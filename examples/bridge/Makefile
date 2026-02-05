# **************************************************************************** #
#    Bridge Example — demonstrates JS <-> C communication                     #
# **************************************************************************** #

NAME		= bridge_demo

SRCS		= main.c
MLX_DIR		= ../../

RAYLIB_WASM_PATH	?= $(HOME)/raylib-wasm

CC_WASM		= emcc
CFLAGS_WASM	= -Wall -Wextra -Os -DPLATFORM_WEB

MLX_LIB_WASM	= $(MLX_DIR)/libmlx_wasm.a
RAYLIB_WASM_LIB	= $(RAYLIB_WASM_PATH)/src/libraylib.web.a

INC_WASM	= -I$(MLX_DIR) -I$(RAYLIB_WASM_PATH)/src

WASM_FLAGS	= -sUSE_GLFW=3 -sASYNCIFY \
		  -sTOTAL_MEMORY=67108864 -sALLOW_MEMORY_GROWTH=1 \
		  -sEXPORTED_RUNTIME_METHODS=allocateUTF8 \
		  -sEXPORTED_FUNCTIONS=_main,_mlx_bridge_restart,_mlx_bridge_clear_args,_mlx_bridge_add_arg,_mlx_bridge_send_command,_mlx_bridge_toggle_fullscreen,_mlx_bridge_toggle_pause

WASM_DIR	= wasm_build

wasm: $(MLX_LIB_WASM) $(RAYLIB_WASM_LIB)
	@mkdir -p $(WASM_DIR)
	$(CC_WASM) $(CFLAGS_WASM) $(INC_WASM) $(SRCS) \
		$(MLX_LIB_WASM) $(RAYLIB_WASM_LIB) \
		$(WASM_FLAGS) \
		--shell-file shell.html \
		-o $(WASM_DIR)/index.html
	@echo "\033[32m✓ WASM bridge demo built\033[0m"

$(MLX_LIB_WASM):
	@$(MAKE) -C $(MLX_DIR) wasm-bridge RAYLIB_WASM_PATH=$(RAYLIB_WASM_PATH)

serve: wasm
	@echo "\033[32m→ http://localhost:8080\033[0m"
	cd $(WASM_DIR) && python3 -m http.server 8080

clean:
	rm -rf $(WASM_DIR)

.PHONY: wasm serve clean
