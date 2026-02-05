/* ************************************************************************** */
/*  examples/basic/main.c                                                     */
/*                                                                            */
/*  Minimal MLX app — animated gradient. Works native and WASM.               */
/*  Build:  make           (native)                                           */
/*          make wasm      (browser)                                          */
/*          make serve     (browser + auto-serve)                             */
/* ************************************************************************** */

#include "mlx.h"

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
} t_app;

static void put_pixel(t_app *app, int x, int y, int color)
{
	char *dst;

	if (x < 0 || x >= WIN_W || y < 0 || y >= WIN_H)
		return;
	dst = app->addr + (y * app->line_len + x * (app->bpp / 8));
	*(unsigned int *)dst = color;
}

static int render(void *param)
{
	t_app *app;
	int x;
	int y;
	int r;
	int g;
	int b;

	app = (t_app *)param;
	y = 0;
	while (y < WIN_H)
	{
		x = 0;
		while (x < WIN_W)
		{
			r = (x + app->frame) % 256;
			g = (y + app->frame / 2) % 256;
			b = ((x + y) / 2 + app->frame) % 256;
			put_pixel(app, x, y, (r << 16) | (g << 8) | b);
			x++;
		}
		y++;
	}
	mlx_put_image_to_window(app->mlx, app->win, app->img, 0, 0);
	app->frame++;
	return (0);
}

static int key_press(int keycode, void *param)
{
	t_app *app;

	app = (t_app *)param;
	if (keycode == 65307)
		mlx_loop_end(app->mlx);
	return (0);
}

int main(void)
{
	t_app app;

	app.frame = 0;
	app.mlx = mlx_init();
	app.win = mlx_new_window(app.mlx, WIN_W, WIN_H, "MLX Basic Example");
	app.img = mlx_new_image(app.mlx, WIN_W, WIN_H);
	app.addr = mlx_get_data_addr(app.img, &app.bpp, &app.line_len,
								 &app.endian);
	mlx_hook(app.win, KeyPress, KeyPressMask, key_press, &app);
	mlx_loop_hook(app.mlx, render, &app);
	mlx_loop(app.mlx);
	mlx_destroy_image(app.mlx, app.img);
	mlx_destroy_window(app.mlx, app.win);
	mlx_destroy_display(app.mlx);
	return (0);
}
