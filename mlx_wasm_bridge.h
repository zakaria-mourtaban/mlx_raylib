/*
** mlx_wasm_bridge.h - Optional WASM <-> JavaScript communication bridge
**
** Include this header and link mlx_wasm_bridge.c to enable browser-side
** control of your MLX application (restart with args, send commands, etc.)
**
** This is entirely optional. Your MLX app works without it.
** When not building for WASM, all functions become no-ops.
*/

#ifndef MLX_WASM_BRIDGE_H
# define MLX_WASM_BRIDGE_H

# define MLX_BRIDGE_MAX_ARGS 32
# define MLX_BRIDGE_MAX_ARG_LEN 256
# define MLX_BRIDGE_MAX_CMD_LEN 512

/*
** Bridge state — tracks pending commands from JS
*/
typedef struct s_bridge_state
{
	int		restart_requested;
	int		argc;
	char	argv[MLX_BRIDGE_MAX_ARGS][MLX_BRIDGE_MAX_ARG_LEN];
	char	pending_command[MLX_BRIDGE_MAX_CMD_LEN];
	int		has_pending_command;
	int		fullscreen_requested;
	int		pause_requested;
	int		is_paused;
}	t_bridge_state;

/*
** Get the global bridge state (singleton)
** Returns a pointer you can poll in your loop_hook
*/
t_bridge_state	*mlx_bridge_get_state(void);

/*
** Check if a restart was requested from JS
** Returns 1 if restart pending, 0 otherwise
** Calling this clears the restart flag
*/
int				mlx_bridge_restart_requested(void);

/*
** Get the new argc/argv after a restart request
** Only valid immediately after mlx_bridge_restart_requested() returns 1
** Returns the new argc, fills argv_out with pointers into internal storage
*/
int				mlx_bridge_get_args(char ***argv_out);

/*
** Check if JS sent a command string
** Returns the command string if pending, NULL otherwise
** Calling this clears the pending command
*/
char			*mlx_bridge_get_command(void);

/*
** Send a status/message from C to JS
** JS can listen for this via Module.onStatusUpdate(msg)
*/
void			mlx_bridge_send_status(const char *status);

/*
** Send a custom event from C to JS with a type and data payload
** JS can listen via Module.onBridgeEvent(type, data)
*/
void			mlx_bridge_send_event(const char *type, const char *data);

/*
** Check if fullscreen was toggled from JS side
** Returns 1 if toggle requested, 0 otherwise (clears flag)
*/
int				mlx_bridge_fullscreen_requested(void);

/*
** Check if pause was toggled from JS side
** Returns 1 if pause toggle requested, 0 otherwise (clears flag)
*/
int				mlx_bridge_pause_requested(void);

#endif /* MLX_WASM_BRIDGE_H */
