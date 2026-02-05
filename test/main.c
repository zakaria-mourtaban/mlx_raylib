/*
** test/main.c - Test program for MinilibX Raylib backend
*/

#include "../mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIN_SX 800
#define WIN_SY 600
#define IMG_SX 400
#define IMG_SY 300

void *g_mlx;
void *g_win;
void *g_img;
char *g_data;
int g_bpp;
int g_sl;
int g_endian;

int key_hook(int keycode, void *param)
{
	(void)param;
	printf("Key pressed: %d\n", keycode);
	if (keycode == 65307 || keycode == 113)
	{
		printf("ESC or Q pressed, exiting...\n");
		mlx_loop_end(g_mlx);
	}
	return (0);
}

int mouse_hook(int button, int x, int y, void *param)
{
	(void)param;
	printf("Mouse button %d at (%d, %d)\n", button, x, y);
	return (0);
}

int motion_hook(int x, int y, void *param)
{
	(void)param;
	printf("Mouse motion at (%d, %d)\n", x, y);
	return (0);
}

int expose_hook(void *param)
{
	(void)param;
	printf("Expose event\n");
	mlx_put_image_to_window(g_mlx, g_win, g_img, 50, 50);
	return (0);
}

int loop_hook(void *param)
{
	(void)param;
	return (0);
}

void fill_image_gradient(void)
{
	int x;
	int y;
	int color;
	int offset;

	for (y = 0; y < IMG_SY; y++)
	{
		for (x = 0; x < IMG_SX; x++)
		{
			color = ((x * 255 / IMG_SX) << 16) |
					((y * 255 / IMG_SY) << 8) |
					(((IMG_SX - x) * 255 / IMG_SX));
			offset = y * g_sl + x * (g_bpp / 8);
			g_data[offset] = color & 0xFF;
			g_data[offset + 1] = (color >> 8) & 0xFF;
			g_data[offset + 2] = (color >> 16) & 0xFF;
			if (g_bpp == 32)
				g_data[offset + 3] = 0xFF;
		}
	}
}

int main(void)
{
	printf("MinilibX Raylib Backend Test\n");
	printf("============================\n\n");

	printf("Initializing MLX...\n");
	g_mlx = mlx_init();
	if (!g_mlx)
	{
		printf("Failed to initialize MLX\n");
		return (1);
	}
	printf("MLX initialized successfully\n");

	printf("Creating window %dx%d...\n", WIN_SX, WIN_SY);
	g_win = mlx_new_window(g_mlx, WIN_SX, WIN_SY, "MLX Raylib Test");
	if (!g_win)
	{
		printf("Failed to create window\n");
		return (1);
	}
	printf("Window created successfully\n");

	printf("Creating image %dx%d...\n", IMG_SX, IMG_SY);
	g_img = mlx_new_image(g_mlx, IMG_SX, IMG_SY);
	if (!g_img)
	{
		printf("Failed to create image\n");
		return (1);
	}
	printf("Image created successfully\n");

	g_data = mlx_get_data_addr(g_img, &g_bpp, &g_sl, &g_endian);
	printf("Image data: bpp=%d, size_line=%d, endian=%d\n", g_bpp, g_sl, g_endian);

	printf("Filling image with gradient...\n");
	fill_image_gradient();

	printf("Drawing some pixels directly...\n");
	mlx_pixel_put(g_mlx, g_win, 10, 10, 0xFF0000);
	mlx_pixel_put(g_mlx, g_win, 11, 10, 0x00FF00);
	mlx_pixel_put(g_mlx, g_win, 12, 10, 0x0000FF);

	printf("Drawing text...\n");
	mlx_string_put(g_mlx, g_win, 100, 500, 0xFFFFFF, "Hello from MLX Raylib!");

	printf("Setting up hooks...\n");
	mlx_key_hook(g_win, key_hook, NULL);
	mlx_mouse_hook(g_win, mouse_hook, NULL);
	mlx_expose_hook(g_win, expose_hook, NULL);
	mlx_hook(g_win, MotionNotify, PointerMotionMask, motion_hook, NULL);
	mlx_loop_hook(g_mlx, loop_hook, NULL);

	printf("\nStarting main loop...\n");
	printf("Press ESC or Q to exit\n\n");
	mlx_loop(g_mlx);

	printf("Cleaning up...\n");
	mlx_destroy_image(g_mlx, g_img);
	mlx_destroy_window(g_mlx, g_win);
	mlx_destroy_display(g_mlx);
	printf("Done!\n");

	return (0);
}
