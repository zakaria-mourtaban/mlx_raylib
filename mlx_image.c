/*
** mlx_image.c - Image management for Raylib backend
*/

#include "mlx_int.h"
#include "mlx.h"

void *mlx_new_image(void *mlx_ptr, int width, int height)
{
	t_img *img;

	(void)mlx_ptr;
	img = (t_img *)calloc(1, sizeof(t_img));
	if (!img)
		return (NULL);
	img->width = width;
	img->height = height;
	img->bpp = 32;
	img->size_line = width * 4;
	img->endian = 0;
	img->data = (char *)calloc(1, height * img->size_line);
	if (!img->data)
	{
		free(img);
		return (NULL);
	}
	img->texture_needs_update = true;
	img->texture.id = 0;
	return (img);
}

char *mlx_get_data_addr(void *img_ptr, int *bits_per_pixel,
						int *size_line, int *endian)
{
	t_img *img;

	img = (t_img *)img_ptr;
	if (!img)
		return (NULL);
	if (bits_per_pixel)
		*bits_per_pixel = img->bpp;
	if (size_line)
		*size_line = img->size_line;
	if (endian)
		*endian = img->endian;
	return (img->data);
}

static void update_texture(t_img *img)
{
	Image raylib_img;
	int x;
	int y;
	int src_offset;
	Color *pixels;

	/* Allocate pixel buffer for conversion */
	pixels = (Color *)malloc(img->width * img->height * sizeof(Color));
	if (!pixels)
		return;

	/* Convert from BGRA (MLX format) to RGBA (Raylib format) */
	for (y = 0; y < img->height; y++)
	{
		for (x = 0; x < img->width; x++)
		{
			src_offset = y * img->size_line + x * 4;
			pixels[y * img->width + x].b = (unsigned char)img->data[src_offset];
			pixels[y * img->width + x].g = (unsigned char)img->data[src_offset + 1];
			pixels[y * img->width + x].r = (unsigned char)img->data[src_offset + 2];
			pixels[y * img->width + x].a = 255;
		}
	}

	/* If texture already exists, update in-place (much faster) */
	if (img->texture.id != 0)
	{
		UpdateTexture(img->texture, pixels);
		free(pixels);
		return;
	}

	/* Otherwise, create new texture */
	raylib_img.data = pixels;
	raylib_img.width = img->width;
	raylib_img.height = img->height;
	raylib_img.mipmaps = 1;
	raylib_img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	img->texture = LoadTextureFromImage(raylib_img);
	free(pixels);
	img->texture_needs_update = false;
}

int mlx_put_image_to_window(void *mlx_ptr, void *win_ptr, void *img_ptr,
							int x, int y)
{
	t_win_list *win;
	t_img *img;

	(void)mlx_ptr;
	win = (t_win_list *)win_ptr;
	img = (t_img *)img_ptr;
	if (!win || !img || !win->active)
		return (0);
	update_texture(img);
	BeginTextureMode(win->render_target);
	DrawTexture(img->texture, x, y, WHITE);
	EndTextureMode();
	return (0);
}

int mlx_destroy_image(void *mlx_ptr, void *img_ptr)
{
	t_img *img;

	(void)mlx_ptr;
	img = (t_img *)img_ptr;
	if (!img)
		return (0);
	if (img->texture.id != 0)
		UnloadTexture(img->texture);
	if (img->data)
		free(img->data);
	free(img);
	return (0);
}

int mlx_get_color_value(void *mlx_ptr, int color)
{
	(void)mlx_ptr;
	return (color);
}
