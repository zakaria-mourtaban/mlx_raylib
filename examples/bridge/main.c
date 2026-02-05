/* ************************************************************************** */
/*  examples/bridge/main.c                                                    */
/*                                                                            */
/*  Demonstrates the WASM bridge — restart with args, send/receive commands.  */
/*  The browser can control this app via JavaScript.                          */
/*                                                                            */
/*  Build:  make wasm      (bridge requires WASM)                             */
/*          make serve     (build + serve)                                    */
/* ************************************************************************** */

#include "mlx.h"
#include "mlx_wasm_bridge.h"
#include <stdio.h>
#include <string.h>

#define WIN_W 800
#define WIN_H 600

typedef struct s_app
{
	void *mlx;
	void *win;
	void *img;
	char *addr;
	int bpp;
	int line_len;
	int endian;
	int frame;
	int color_mode;
	char *scene_name;
} t_app;

static void put_pixel(t_app *app, int x, int y, int color)
{
	char *dst;

	if (x < 0 || x >= WIN_W || y < 0 || y >= WIN_H)
		return;
	dst = app->addr + (y * app->line_len + x * (app->bpp / 8));
	*(unsigned int *)dst = color;
}

static int make_color(t_app *app, int x, int y)
{
	int r;
	int g;
	int b;

	if (app->color_mode == 0)
	{
		r = (x + app->frame) % 256;
		g = (y + app->frame / 2) % 256;
		b = ((x + y) / 2 + app->frame) % 256;
	}
	else if (app->color_mode == 1)
	{
		r = (x * 256 / WIN_W);
		g = 0;
		b = (y * 256 / WIN_H);
	}
	else
	{
		r = ((x ^ y) + app->frame) % 256;
		g = ((x & y) + app->frame) % 256;
		b = ((x | y) + app->frame) % 256;
	}
	return ((r << 16) | (g << 8) | b);
}

static int render(void *param)
{
	t_app *app;
	int x;
	int y;
	char *cmd;

	app = (t_app *)param;
	/* ── Check bridge for restart request ── */
	if (mlx_bridge_restart_requested())
	{
		char **argv;
		int argc;

		argc = mlx_bridge_get_args(&argv);
		printf("Restart requested with %d args\n", argc);
		if (argc > 1)
		{
			app->scene_name = argv[1];
			printf("New scene: %s\n", app->scene_name);
		}
		app->frame = 0;
		mlx_bridge_send_status("Restarted!");
	}
	/* ── Check bridge for commands ── */
	cmd = mlx_bridge_get_command();
	if (cmd)
	{
		printf("Command received: %s\n", cmd);
		if (strcmp(cmd, "mode0") == 0)
			app->color_mode = 0;
		else if (strcmp(cmd, "mode1") == 0)
			app->color_mode = 1;
		else if (strcmp(cmd, "mode2") == 0)
			app->color_mode = 2;
		else if (strcmp(cmd, "reset") == 0)
			app->frame = 0;
		mlx_bridge_send_event("mode_changed", cmd);
	}
	/* ── Render ── */
	y = -1;
	while (++y < WIN_H)
	{
		x = -1;
		while (++x < WIN_W)
			put_pixel(app, x, y, make_color(app, x, y));
	}
	mlx_put_image_to_window(app->mlx, app->win, app->img, 0, 0);
	app->frame++;
	/* Send frame count every 60 frames */
	if (app->frame % 60 == 0)
	{
		char buf[64];

		snprintf(buf, sizeof(buf), "Frame %d", app->frame);
		mlx_bridge_send_status(buf);
	}
	return (0);
}

static int key_press(int keycode, void *param)
{
	t_app *app;

	app = (t_app *)param;
	if (keycode == 65307)
		mlx_loop_end(app->mlx);
	else if (keycode == 49)
		app->color_mode = 0;
	else if (keycode == 50)
		app->color_mode = 1;
	else if (keycode == 51)
		app->color_mode = 2;
	return (0);
}

int main(int argc, char **argv)
{
	t_app app;

	memset(&app, 0, sizeof(app));
	if (argc > 1)
		app.scene_name = argv[1];
	else
		app.scene_name = "default";
	printf("Starting with scene: %s\n", app.scene_name);
	mlx_bridge_send_status("Initializing...");
	app.mlx = mlx_init();
	app.win = mlx_new_window(app.mlx, WIN_W, WIN_H, "Bridge Demo");
	app.img = mlx_new_image(app.mlx, WIN_W, WIN_H);
	app.addr = mlx_get_data_addr(app.img, &app.bpp, &app.line_len,
								 &app.endian);
	mlx_hook(app.win, KeyPress, KeyPressMask, key_press, &app);
	mlx_loop_hook(app.mlx, render, &app);
	mlx_bridge_send_status("Running");
	mlx_loop(app.mlx);
	mlx_destroy_image(app.mlx, app.img);
	mlx_destroy_window(app.mlx, app.win);
	mlx_destroy_display(app.mlx);
	return (0);
}
