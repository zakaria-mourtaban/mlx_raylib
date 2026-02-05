/*
** mlx_mouse.c - Mouse functions for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

int mlx_mouse_get_pos(void *mlx_ptr, void *win_ptr, int *x, int *y)
{
	(void)mlx_ptr;
	(void)win_ptr;
	if (x)
		*x = GetMouseX();
	if (y)
		*y = GetMouseY();
	return (0);
}

int mlx_mouse_move(void *mlx_ptr, void *win_ptr, int x, int y)
{
	(void)mlx_ptr;
	(void)win_ptr;
	SetMousePosition(x, y);
	return (0);
}

int mlx_mouse_hide(void *mlx_ptr, void *win_ptr)
{
	t_xvar *xvar;

	(void)win_ptr;
	xvar = (t_xvar *)mlx_ptr;
	if (xvar)
		xvar->mouse_hidden = true;
	HideCursor();
	return (0);
}

int mlx_mouse_show(void *mlx_ptr, void *win_ptr)
{
	t_xvar *xvar;

	(void)win_ptr;
	xvar = (t_xvar *)mlx_ptr;
	if (xvar)
		xvar->mouse_hidden = false;
	ShowCursor();
	return (0);
}
