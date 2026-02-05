/*
** mlx_pixel_put.c - Pixel and drawing functions for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

int mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color)
{
	t_win_list *win;
	Color c;

	(void)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!win || !win->active)
		return (0);
	c.r = (color >> 16) & 0xFF;
	c.g = (color >> 8) & 0xFF;
	c.b = color & 0xFF;
	c.a = 255;
	BeginTextureMode(win->render_target);
	DrawPixel(x, y, c);
	EndTextureMode();
	return (0);
}

int mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y, int color,
				   char *string)
{
	t_win_list *win;
	Color c;

	(void)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!win || !win->active || !string)
		return (0);
	c.r = (color >> 16) & 0xFF;
	c.g = (color >> 8) & 0xFF;
	c.b = color & 0xFF;
	c.a = 255;
	BeginTextureMode(win->render_target);
	DrawText(string, x, y, 10, c);
	EndTextureMode();
	return (0);
}

void mlx_set_font(void *mlx_ptr, void *win_ptr, char *name)
{
	(void)mlx_ptr;
	(void)win_ptr;
	(void)name;
}
