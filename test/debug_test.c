#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct s_app {
    void *mlx;
    void *win;
    void *img;
    char *data;
    int bpp;
    int size_line;
    int endian;
    int frame;
} t_app;

static t_app g_app;

int loop_hook(void *param)
{
    t_app *app = (t_app *)param;
    int x, y;
    for (y = 0; y < 600; y++) {
        for (x = 0; x < 800; x++) {
            int offset = y * app->size_line + x * (app->bpp / 8);
            unsigned int color = ((x + app->frame) % 256) << 16 | (y % 256) << 8 | 128;
            *(unsigned int *)(app->data + offset) = color;
        }
    }
    mlx_put_image_to_window(app->mlx, app->win, app->img, 0, 0);
    app->frame++;
    return 0;
}

int main(void)
{
    printf("Starting debug test...\n");
    g_app.frame = 0;
    g_app.mlx = mlx_init();
    printf("mlx_init returned: %p\n", g_app.mlx);
    if (!g_app.mlx) return 1;
    g_app.win = mlx_new_window(g_app.mlx, 800, 600, "Debug Test");
    printf("mlx_new_window returned: %p\n", g_app.win);
    if (!g_app.win) return 1;
    g_app.img = mlx_new_image(g_app.mlx, 800, 600);
    printf("mlx_new_image returned: %p\n", g_app.img);
    if (!g_app.img) return 1;
    g_app.data = mlx_get_data_addr(g_app.img, &g_app.bpp, &g_app.size_line, &g_app.endian);
    printf("bpp=%d size_line=%d\n", g_app.bpp, g_app.size_line);
    printf("Calling mlx_loop...\n");
    mlx_loop_hook(g_app.mlx, loop_hook, &g_app);
    mlx_loop(g_app.mlx);
    printf("mlx_loop returned\n");
    return 0;
}
