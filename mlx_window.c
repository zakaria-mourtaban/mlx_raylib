/*
** mlx_window.c - Window management for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

void *mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title)
{
	t_xvar *xvar;
	t_win_list *win;
	t_win_list *last;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (NULL);
	win = (t_win_list *)calloc(1, sizeof(t_win_list));
	if (!win)
		return (NULL);
	if (!xvar->initialized)
	{
		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		InitWindow(size_x, size_y, title);
		SetTargetFPS(60);
		xvar->initialized = true;
		xvar->screen_width = size_x;
		xvar->screen_height = size_y;
	}
	win->width = size_x;
	win->height = size_y;
	win->title = strdup(title ? title : "MLX Window");
	win->active = true;
	win->render_target = LoadRenderTexture(size_x, size_y);
	win->next = NULL;
	win->mouse_hook = NULL;
	win->key_hook = NULL;
	win->expose_hook = NULL;
	for (int i = 0; i < MLX_MAX_EVENT; i++)
	{
		win->hooks[i].hook = NULL;
		win->hooks[i].param = NULL;
		win->hooks[i].mask = 0;
	}
	if (!xvar->win_list)
		xvar->win_list = win;
	else
	{
		last = xvar->win_list;
		while (last->next)
			last = last->next;
		last->next = win;
	}
	if (!xvar->active_window)
		xvar->active_window = win;
	BeginTextureMode(win->render_target);
	ClearBackground(BLACK);
	EndTextureMode();
	return (win);
}

int mlx_destroy_window(void *mlx_ptr, void *win_ptr)
{
	t_xvar *xvar;
	t_win_list *win;
	t_win_list *prev;

	xvar = (t_xvar *)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!xvar || !win)
		return (0);
	if (win->active)
	{
		UnloadRenderTexture(win->render_target);
		win->active = false;
	}
	if (win->title)
	{
		free(win->title);
		win->title = NULL;
	}
	prev = NULL;
	if (xvar->win_list == win)
		xvar->win_list = win->next;
	else
	{
		prev = xvar->win_list;
		while (prev && prev->next != win)
			prev = prev->next;
		if (prev)
			prev->next = win->next;
	}
	if (xvar->active_window == win)
		xvar->active_window = xvar->win_list;
	free(win);
	return (0);
}

int mlx_clear_window(void *mlx_ptr, void *win_ptr)
{
	t_win_list *win;

	(void)mlx_ptr;
	win = (t_win_list *)win_ptr;
	if (!win || !win->active)
		return (0);
	BeginTextureMode(win->render_target);
	ClearBackground(BLACK);
	EndTextureMode();
	return (0);
}
