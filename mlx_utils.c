/*
** mlx_utils.c - Utility functions for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

int mlx_get_screen_size(void *mlx_ptr, int *sizex, int *sizey)
{
	(void)mlx_ptr;
	if (sizex)
		*sizex = GetScreenWidth();
	if (sizey)
		*sizey = GetScreenHeight();
	return (0);
}

int mlx_do_key_autorepeatoff(void *mlx_ptr)
{
	t_xvar *xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (xvar)
		xvar->key_autorepeat = false;
	return (0);
}

int mlx_do_key_autorepeaton(void *mlx_ptr)
{
	t_xvar *xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (xvar)
		xvar->key_autorepeat = true;
	return (0);
}

int mlx_do_sync(void *mlx_ptr)
{
	(void)mlx_ptr;
	return (0);
}
