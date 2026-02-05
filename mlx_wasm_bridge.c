/*
** mlx_wasm_bridge.c - Optional WASM <-> JavaScript communication bridge
**
** When compiled for WASM (__EMSCRIPTEN__), this exposes C functions callable
** from JavaScript and provides C functions to send data back to JS.
**
** When compiled natively, all functions are safe no-ops.
*/

#include "mlx_wasm_bridge.h"
#include <string.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* Singleton bridge state */
static t_bridge_state g_bridge = {0};

t_bridge_state *mlx_bridge_get_state(void)
{
	return (&g_bridge);
}

int mlx_bridge_restart_requested(void)
{
	int val;

	val = g_bridge.restart_requested;
	g_bridge.restart_requested = 0;
	return (val);
}

int mlx_bridge_get_args(char ***argv_out)
{
	static char *ptrs[MLX_BRIDGE_MAX_ARGS];
	int i;

	i = 0;
	while (i < g_bridge.argc)
	{
		ptrs[i] = g_bridge.argv[i];
		i++;
	}
	*argv_out = ptrs;
	return (g_bridge.argc);
}

char *mlx_bridge_get_command(void)
{
	if (!g_bridge.has_pending_command)
		return (NULL);
	g_bridge.has_pending_command = 0;
	return (g_bridge.pending_command);
}

int mlx_bridge_fullscreen_requested(void)
{
	int val;

	val = g_bridge.fullscreen_requested;
	g_bridge.fullscreen_requested = 0;
	return (val);
}

int mlx_bridge_pause_requested(void)
{
	int val;

	val = g_bridge.pause_requested;
	g_bridge.pause_requested = 0;
	return (val);
}

/*
** === Functions exported to JavaScript (WASM only) ===
** These are called from JS via Module._mlx_bridge_*()
*/

#ifdef __EMSCRIPTEN__

/*
** JS calls this to request a restart with new arguments.
** Usage from JS: Module._mlx_bridge_restart()
**   after calling mlx_bridge_set_arg() for each argument
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_restart(void)
{
	g_bridge.restart_requested = 1;
}

/*
** JS calls this to clear args before setting new ones.
** Usage from JS: Module._mlx_bridge_clear_args()
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_clear_args(void)
{
	g_bridge.argc = 0;
	memset(g_bridge.argv, 0, sizeof(g_bridge.argv));
}

/*
** JS calls this to add an argument string.
** The string must be passed as a pointer to WASM memory.
** Usage from JS:
**   var ptr = Module.allocateUTF8("path/to/scene.rt");
**   Module._mlx_bridge_add_arg(ptr);
**   Module._free(ptr);
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_add_arg(const char *arg)
{
	if (g_bridge.argc < MLX_BRIDGE_MAX_ARGS && arg)
	{
		strncpy(g_bridge.argv[g_bridge.argc], arg,
				MLX_BRIDGE_MAX_ARG_LEN - 1);
		g_bridge.argv[g_bridge.argc][MLX_BRIDGE_MAX_ARG_LEN - 1] = '\0';
		g_bridge.argc++;
	}
}

/*
** JS calls this to send a command string to C.
** Usage from JS:
**   var ptr = Module.allocateUTF8("toggle_debug");
**   Module._mlx_bridge_send_command(ptr);
**   Module._free(ptr);
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_send_command(const char *cmd)
{
	if (cmd)
	{
		strncpy(g_bridge.pending_command, cmd,
				MLX_BRIDGE_MAX_CMD_LEN - 1);
		g_bridge.pending_command[MLX_BRIDGE_MAX_CMD_LEN - 1] = '\0';
		g_bridge.has_pending_command = 1;
	}
}

/*
** JS calls this to request fullscreen toggle.
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_toggle_fullscreen(void)
{
	g_bridge.fullscreen_requested = 1;
}

/*
** JS calls this to request pause toggle.
*/
EMSCRIPTEN_KEEPALIVE
void mlx_bridge_toggle_pause(void)
{
	g_bridge.pause_requested = 1;
}

#endif /* __EMSCRIPTEN__ */

/*
** === Functions to send data FROM C TO JavaScript ===
*/

void mlx_bridge_send_status(const char *status)
{
#ifdef __EMSCRIPTEN__
	EM_ASM({
		if (Module.onStatusUpdate)
			Module.onStatusUpdate(UTF8ToString($0)); }, status);
#else
	printf("[status] %s\n", status);
#endif
}

void mlx_bridge_send_event(const char *type, const char *data)
{
#ifdef __EMSCRIPTEN__
	EM_ASM({
		if (Module.onBridgeEvent)
			Module.onBridgeEvent(UTF8ToString($0), UTF8ToString($1)); }, type, data);
#else
	printf("[event:%s] %s\n", type, data);
#endif
}
