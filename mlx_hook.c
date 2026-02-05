/*
** mlx_hook.c - Event hooks for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

int mlx_mouse_hook(void *win_ptr, int (*funct_ptr)(), void *param)
{
	t_win_list *win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->mouse_hook = funct_ptr;
	win->mouse_param = param;
	return (0);
}

int mlx_key_hook(void *win_ptr, int (*funct_ptr)(), void *param)
{
	t_win_list *win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->key_hook = funct_ptr;
	win->key_param = param;
	return (0);
}

int mlx_expose_hook(void *win_ptr, int (*funct_ptr)(), void *param)
{
	t_win_list *win;

	win = (t_win_list *)win_ptr;
	if (!win)
		return (0);
	win->expose_hook = funct_ptr;
	win->expose_param = param;
	return (0);
}

int mlx_loop_hook(void *mlx_ptr, int (*funct_ptr)(), void *param)
{
	t_xvar *xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	xvar->loop_hook = funct_ptr;
	xvar->loop_param = param;
	return (0);
}

int mlx_hook(void *win_ptr, int x_event, int x_mask,
			 int (*funct)(), void *param)
{
	t_win_list *win;

	win = (t_win_list *)win_ptr;
	if (!win || x_event < 0 || x_event >= MLX_MAX_EVENT)
		return (0);
	win->hooks[x_event].hook = funct;
	win->hooks[x_event].param = param;
	win->hooks[x_event].mask = x_mask;
	return (0);
}
