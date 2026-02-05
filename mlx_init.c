/*
** mlx_init.c - Initialize MLX context with Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

static t_xvar *g_mlx = NULL;

void *mlx_init(void)
{
	t_xvar *xvar;

	if (g_mlx != NULL)
		return (g_mlx);
	xvar = (t_xvar *)calloc(1, sizeof(t_xvar));
	if (!xvar)
		return (NULL);
	xvar->screen_width = 1920;
	xvar->screen_height = 1080;
	xvar->win_list = NULL;
	xvar->active_window = NULL;
	xvar->loop_hook = NULL;
	xvar->loop_param = NULL;
	xvar->end_loop = false;
	xvar->key_autorepeat = true;
	xvar->mouse_hidden = false;
	xvar->use_xshm = 1;
	xvar->pshm_format = 2;
	xvar->initialized = false;
	g_mlx = xvar;
	return (xvar);
}

int mlx_destroy_display(void *mlx_ptr)
{
	t_xvar *xvar;
	t_win_list *win;
	t_win_list *next;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar)
		return (0);
	win = xvar->win_list;
	while (win)
	{
		next = win->next;
		if (win->active)
			UnloadRenderTexture(win->render_target);
		if (win->title)
			free(win->title);
		free(win);
		win = next;
	}
	if (xvar->initialized)
		CloseWindow();
	free(xvar);
	if (g_mlx == xvar)
		g_mlx = NULL;
	return (0);
}
