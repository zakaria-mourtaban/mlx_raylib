/*
** WASM Test - Simple MLX Raylib test for browser
** Uses mlx_loop which handles WASM internally
*/

#include "mlx.h"
#include <stdlib.h>

typedef struct s_app
{
void *mlx;
void *win;
void *img;
char *data;
int bpp;
int size_line;
int endian;
int width;
int height;
int frame;
} t_app;

void put_pixel(t_app *app, int x, int y, int color)
{
char *dst;

if (x < 0 || x >= app->width || y < 0 || y >= app->height)
return;
dst = app->data + (y * app->size_line + x * (app->bpp / 8));
*(unsigned int *)dst = color;
}

void render_frame(t_app *app)
{
int x, y;
int color;

/* Animated gradient based on frame count */
for (y = 0; y < app->height; y++)
{
for (x = 0; x < app->width; x++)
{
int r = (x + app->frame) % 256;
int g = (y + app->frame / 2) % 256;
int b = ((x + y + app->frame) / 2) % 256;
color = (r << 16) | (g << 8) | b;
put_pixel(app, x, y, color);
}
}

/* Draw a bouncing circle */
int cx = app->width / 2 + (app->frame * 2) % (app->width / 2) - app->width / 4;
int cy = app->height / 2 + (app->frame * 3) % (app->height / 2) - app->height / 4;
int radius = 50;

for (y = -radius; y <= radius; y++)
{
for (x = -radius; x <= radius; x++)
{
if (x * x + y * y <= radius * radius)
put_pixel(app, cx + x, cy + y, 0xFFFFFF);
}
}

mlx_put_image_to_window(app->mlx, app->win, app->img, 0, 0);
app->frame++;
}

int loop_hook(void *param)
{
t_app *app = (t_app *)param;
render_frame(app);
return (0);
}

int key_hook(int keycode, void *param)
{
t_app *app = (t_app *)param;
if (keycode == 65307 || keycode == 113) /* ESC or Q */
mlx_loop_end(app->mlx);
return (0);
}

int main(void)
{
t_app app;

app.width = 800;
app.height = 600;
app.frame = 0;

app.mlx = mlx_init();
if (!app.mlx)
return (1);

app.win = mlx_new_window(app.mlx, app.width, app.height, "MLX WASM Test");
if (!app.win)
return (1);

app.img = mlx_new_image(app.mlx, app.width, app.height);
if (!app.img)
return (1);

app.data = mlx_get_data_addr(app.img, &app.bpp, &app.size_line, &app.endian);

/* Use mlx_loop - it handles WASM internally with emscripten_set_main_loop */
mlx_hook(app.win, 2, 1L << 0, key_hook, &app);
mlx_loop_hook(app.mlx, loop_hook, &app);
mlx_loop(app.mlx);

return (0);
}
