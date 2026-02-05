/*
** mlx_loop.c - Main event loop for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static void process_key_events(t_xvar *xvar)
{
	t_win_list *win;
	int key;
	int x11_key;

	/* Use GetKeyPressed queue for key press events (better for WASM) */
	key = GetKeyPressed();
	while (key > 0)
	{
		x11_key = mlx_raylib_to_x11_key(key);
		win = xvar->win_list;
		while (win)
		{
			if (win->active && win->hooks[KeyPress].hook)
				win->hooks[KeyPress].hook(x11_key, win->hooks[KeyPress].param);
			if (win->active && win->key_hook)
				win->key_hook(x11_key, win->key_param);
			win = win->next;
		}
		key = GetKeyPressed();
	}
	/* Check key releases by polling */
	for (key = KEY_SPACE; key <= KEY_KP_EQUAL; key++)
	{
		if (IsKeyReleased(key))
		{
			x11_key = mlx_raylib_to_x11_key(key);
			win = xvar->win_list;
			while (win)
			{
				if (win->active && win->hooks[KeyRelease].hook)
					win->hooks[KeyRelease].hook(x11_key,
												win->hooks[KeyRelease].param);
				win = win->next;
			}
		}
	}
}

static void process_mouse_button_events(t_xvar *xvar)
{
	t_win_list *win;
	int button;
	int x;
	int y;

	x = GetMouseX();
	y = GetMouseY();
	for (button = MOUSE_BUTTON_LEFT; button <= MOUSE_BUTTON_MIDDLE; button++)
	{
		if (IsMouseButtonPressed(button))
		{
			win = xvar->win_list;
			while (win)
			{
				if (win->active && win->hooks[ButtonPress].hook)
					win->hooks[ButtonPress].hook(button + 1, x, y,
												 win->hooks[ButtonPress].param);
				if (win->active && win->mouse_hook)
					win->mouse_hook(button + 1, x, y, win->mouse_param);
				win = win->next;
			}
		}
		if (IsMouseButtonReleased(button))
		{
			win = xvar->win_list;
			while (win)
			{
				if (win->active && win->hooks[ButtonRelease].hook)
					win->hooks[ButtonRelease].hook(button + 1, x, y,
												   win->hooks[ButtonRelease].param);
				win = win->next;
			}
		}
	}
}

static void process_mouse_motion(t_xvar *xvar)
{
	t_win_list *win;
	int x;
	int y;
	static int last_x = -1;
	static int last_y = -1;

	x = GetMouseX();
	y = GetMouseY();
	if (x != last_x || y != last_y)
	{
		last_x = x;
		last_y = y;
		win = xvar->win_list;
		while (win)
		{
			if (win->active && win->hooks[MotionNotify].hook)
				win->hooks[MotionNotify].hook(x, y, win->hooks[MotionNotify].param);
			win = win->next;
		}
	}
}

static void render_windows(t_xvar *xvar)
{
	t_win_list *win;
	Rectangle src_rect;
	Rectangle dst_rect;

	BeginDrawing();
	ClearBackground(BLACK);
	win = xvar->active_window;
	if (win && win->active)
	{
		src_rect = (Rectangle){0, 0, (float)win->width, (float)-win->height};
		dst_rect = (Rectangle){0, 0, (float)GetScreenWidth(),
							   (float)GetScreenHeight()};
		DrawTexturePro(win->render_target.texture, src_rect, dst_rect,
					   (Vector2){0, 0}, 0.0f, WHITE);
	}
	EndDrawing();
}

static void call_expose_hooks(t_xvar *xvar)
{
	t_win_list *win;

	win = xvar->win_list;
	while (win)
	{
		if (win->active && win->expose_hook)
			win->expose_hook(win->expose_param);
		if (win->active && win->hooks[Expose].hook)
			win->hooks[Expose].hook(win->hooks[Expose].param);
		win = win->next;
	}
}

/* Global pointer for Emscripten callback */
#ifdef __EMSCRIPTEN__
static t_xvar *g_xvar = NULL;
static bool g_first_frame = true;

static void mlx_loop_iteration(void)
{
	if (!g_xvar || !g_xvar->initialized)
		return;
	if (g_first_frame)
	{
		call_expose_hooks(g_xvar);
		g_first_frame = false;
	}
	process_key_events(g_xvar);
	process_mouse_button_events(g_xvar);
	process_mouse_motion(g_xvar);
	if (g_xvar->loop_hook)
		g_xvar->loop_hook(g_xvar->loop_param);
	render_windows(g_xvar);
}
#endif

int mlx_loop(void *mlx_ptr)
{
	t_xvar *xvar;
	bool first_frame;

	xvar = (t_xvar *)mlx_ptr;
	if (!xvar || !xvar->initialized)
		return (0);
	xvar->end_loop = false;
#ifdef __EMSCRIPTEN__
	g_xvar = xvar;
	g_first_frame = true;
	emscripten_set_main_loop(mlx_loop_iteration, 0, 1);
#else
	first_frame = true;
	while (!WindowShouldClose() && !xvar->end_loop)
	{
		if (first_frame)
		{
			call_expose_hooks(xvar);
			first_frame = false;
		}
		process_key_events(xvar);
		process_mouse_button_events(xvar);
		process_mouse_motion(xvar);
		if (xvar->loop_hook)
			xvar->loop_hook(xvar->loop_param);
		render_windows(xvar);
	}
#endif
	return (0);
}

int mlx_loop_end(void *mlx_ptr)
{
	t_xvar *xvar;

	xvar = (t_xvar *)mlx_ptr;
	if (xvar)
		xvar->end_loop = true;
#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();
#endif
	return (0);
}
