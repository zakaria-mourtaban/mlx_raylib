/*
** mlx_xpm.c - XPM image loading for Raylib backend
** Note: This is a simplified XPM parser, may not handle all XPM formats
*/

#include "mlx_int.h"
#include "mlx.h"
#include <ctype.h>

static int parse_xpm_header(char **xpm_data, int *width, int *height,
							int *num_colors, int *chars_per_pixel)
{
	char *header;
	int values[4];
	int vi;

	header = xpm_data[0];
	while (*header && !isdigit(*header))
		header++;
	vi = 0;
	while (*header && vi < 4)
	{
		values[vi] = 0;
		while (*header && isdigit(*header))
		{
			values[vi] = values[vi] * 10 + (*header - '0');
			header++;
		}
		vi++;
		while (*header && !isdigit(*header))
			header++;
	}
	*width = values[0];
	*height = values[1];
	*num_colors = values[2];
	*chars_per_pixel = values[3];
	return (0);
}

static int hex_char_to_int(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (0);
}

static int parse_color(char *color_str)
{
	int color;
	char *p;

	color = 0;
	p = strstr(color_str, "#");
	if (p)
	{
		p++;
		for (int i = 0; i < 6 && p[i]; i++)
			color = (color << 4) | hex_char_to_int(p[i]);
	}
	return (color);
}

void *mlx_xpm_to_image(void *mlx_ptr, char **xpm_data,
					   int *width, int *height)
{
	t_img *img;
	int w;
	int h;
	int num_colors;
	int cpp;
	int *color_table;
	char *key_table;
	int i;
	int x;
	int y;
	int offset;
	int color;
	char *pixel_key;
	char *row;

	if (!xpm_data)
		return (NULL);
	parse_xpm_header(xpm_data, &w, &h, &num_colors, &cpp);
	if (width)
		*width = w;
	if (height)
		*height = h;
	color_table = (int *)malloc(num_colors * sizeof(int));
	key_table = (char *)malloc(num_colors * cpp);
	if (!color_table || !key_table)
	{
		free(color_table);
		free(key_table);
		return (NULL);
	}
	for (i = 0; i < num_colors; i++)
	{
		memcpy(key_table + i * cpp, xpm_data[1 + i], cpp);
		color_table[i] = parse_color(xpm_data[1 + i] + cpp);
	}
	img = (t_img *)mlx_new_image(mlx_ptr, w, h);
	if (!img)
	{
		free(color_table);
		free(key_table);
		return (NULL);
	}
	pixel_key = (char *)malloc(cpp + 1);
	pixel_key[cpp] = '\0';
	for (y = 0; y < h; y++)
	{
		row = xpm_data[1 + num_colors + y];
		for (x = 0; x < w; x++)
		{
			memcpy(pixel_key, row + x * cpp, cpp);
			color = 0;
			for (i = 0; i < num_colors; i++)
			{
				if (memcmp(pixel_key, key_table + i * cpp, cpp) == 0)
				{
					color = color_table[i];
					break;
				}
			}
			offset = y * img->size_line + x * 4;
			img->data[offset] = color & 0xFF;
			img->data[offset + 1] = (color >> 8) & 0xFF;
			img->data[offset + 2] = (color >> 16) & 0xFF;
			img->data[offset + 3] = 0xFF;
		}
	}
	free(pixel_key);
	free(color_table);
	free(key_table);
	img->texture_needs_update = true;
	return (img);
}

static char **read_xpm_file(char *filename, int *line_count)
{
	FILE *fp;
	char **lines;
	char buffer[4096];
	int capacity;
	int count;
	char *p;
	int in_data;
	int len;

	fp = fopen(filename, "r");
	if (!fp)
		return (NULL);
	capacity = 256;
	count = 0;
	lines = (char **)malloc(capacity * sizeof(char *));
	in_data = 0;
	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (strstr(buffer, "static char") || strstr(buffer, "static const char"))
		{
			in_data = 1;
			continue;
		}
		if (!in_data)
			continue;
		p = strchr(buffer, '"');
		if (!p)
			continue;
		p++;
		len = 0;
		while (p[len] && p[len] != '"')
			len++;
		if (count >= capacity - 1)
		{
			capacity *= 2;
			lines = (char **)realloc(lines, capacity * sizeof(char *));
		}
		lines[count] = (char *)malloc(len + 1);
		memcpy(lines[count], p, len);
		lines[count][len] = '\0';
		count++;
	}
	fclose(fp);
	lines[count] = NULL;
	*line_count = count;
	return (lines);
}

void *mlx_xpm_file_to_image(void *mlx_ptr, char *filename,
							int *width, int *height)
{
	char **xpm_data;
	int line_count;
	void *img;
	int i;

	xpm_data = read_xpm_file(filename, &line_count);
	if (!xpm_data)
		return (NULL);
	img = mlx_xpm_to_image(mlx_ptr, xpm_data, width, height);
	for (i = 0; i < line_count; i++)
		free(xpm_data[i]);
	free(xpm_data);
	return (img);
}
