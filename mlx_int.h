/*
** mlx_int.h - Internal structures for MinilibX Raylib backend
*/

#ifndef MLX_INT_H
# define MLX_INT_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>

/* For strdup on strict C99 */
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif

# include "raylib.h"

# define MLX_MAX_EVENT 35
# define MLX_MAX_WINDOWS 16

/*
** Event list structure for hooks
*/
typedef struct s_event_list
{
	int		mask;
	int		(*hook)();
	void	*param;
}	t_event_list;

/*
** Window structure
*/
typedef struct s_win_list
{
	int					id;
	int					width;
	int					height;
	char				*title;
	bool				active;
	RenderTexture2D		render_target;
	struct s_win_list	*next;
	int					(*mouse_hook)();
	int					(*key_hook)();
	int					(*expose_hook)();
	void				*mouse_param;
	void				*key_param;
	void				*expose_param;
	t_event_list		hooks[MLX_MAX_EVENT];
}	t_win_list;

/*
** Image structure
*/
typedef struct s_img
{
	int				width;
	int				height;
	int				size_line;
	int				bpp;
	int				endian;
	char			*data;
	Texture2D		texture;
	bool			texture_needs_update;
}	t_img;

/*
** Main MLX context structure
*/
typedef struct s_xvar
{
	bool			initialized;
	int				screen_width;
	int				screen_height;
	t_win_list		*win_list;
	t_win_list		*active_window;
	int				(*loop_hook)();
	void			*loop_param;
	bool			end_loop;
	bool			key_autorepeat;
	bool			mouse_hidden;
	int				use_xshm;
	int				pshm_format;
}	t_xvar;

/*
** Raylib to X11 keycode mapping
*/
int		mlx_raylib_to_x11_key(int raylib_key);
int		mlx_x11_to_raylib_key(int x11_key);

#endif /* MLX_INT_H */
