/*
** mlx.h - MinilibX compatible header for Raylib backend
**
** This is a drop-in replacement for MinilibX using Raylib
** Supports compilation to WASM for web deployment
*/

#ifndef MLX_H
#define MLX_H

/*
** Core initialization
*/
void *mlx_init(void);

/*
** Window management
*/
void *mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title);
int mlx_clear_window(void *mlx_ptr, void *win_ptr);
int mlx_destroy_window(void *mlx_ptr, void *win_ptr);

/*
** Pixel operations
*/
int mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color);

/*
** Image management
*/
void *mlx_new_image(void *mlx_ptr, int width, int height);
char *mlx_get_data_addr(void *img_ptr, int *bits_per_pixel,
						int *size_line, int *endian);
int mlx_put_image_to_window(void *mlx_ptr, void *win_ptr, void *img_ptr,
							int x, int y);
int mlx_destroy_image(void *mlx_ptr, void *img_ptr);

int mlx_destroy_display(void *mlx_ptr);

int mlx_get_color_value(void *mlx_ptr, int color);

/*
** XPM image loading (limited support)
*/
void *mlx_xpm_to_image(void *mlx_ptr, char **xpm_data,
					   int *width, int *height);
void *mlx_xpm_file_to_image(void *mlx_ptr, char *filename,
							int *width, int *height);

/*
** Event hooks
*/
int mlx_mouse_hook(void *win_ptr, int (*funct_ptr)(), void *param);
int mlx_key_hook(void *win_ptr, int (*funct_ptr)(), void *param);
int mlx_expose_hook(void *win_ptr, int (*funct_ptr)(), void *param);
int mlx_loop_hook(void *mlx_ptr, int (*funct_ptr)(), void *param);
int mlx_hook(void *win_ptr, int x_event, int x_mask,
			 int (*funct)(), void *param);

/*
** Main loop
*/
int mlx_loop(void *mlx_ptr);
int mlx_loop_end(void *mlx_ptr);

/*
** Text rendering
*/
int mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y, int color,
				   char *string);
void mlx_set_font(void *mlx_ptr, void *win_ptr, char *name);

/*
** Keyboard controls
*/
int mlx_do_key_autorepeatoff(void *mlx_ptr);
int mlx_do_key_autorepeaton(void *mlx_ptr);

/*
** Sync
*/
int mlx_do_sync(void *mlx_ptr);

/*
** Mouse controls
*/
int mlx_mouse_get_pos(void *mlx_ptr, void *win_ptr, int *x, int *y);
int mlx_mouse_move(void *mlx_ptr, void *win_ptr, int x, int y);
int mlx_mouse_hide(void *mlx_ptr, void *win_ptr);
int mlx_mouse_show(void *mlx_ptr, void *win_ptr);

/*
** Screen info
*/
int mlx_get_screen_size(void *mlx_ptr, int *sizex, int *sizey);

/*
** X11 event compatibility defines
** These match X11/X.h values for compatibility with existing code
*/
#define KeyPress 2
#define KeyRelease 3
#define ButtonPress 4
#define ButtonRelease 5
#define MotionNotify 6
#define EnterNotify 7
#define LeaveNotify 8
#define FocusIn 9
#define FocusOut 10
#define KeymapNotify 11
#define Expose 12
#define GraphicsExpose 13
#define NoExpose 14
#define VisibilityNotify 15
#define CreateNotify 16
#define DestroyNotify 17
#define UnmapNotify 18
#define MapNotify 19
#define MapRequest 20
#define ReparentNotify 21
#define ConfigureNotify 22
#define ConfigureRequest 23
#define GravityNotify 24
#define ResizeRequest 25
#define CirculateNotify 26
#define CirculateRequest 27
#define PropertyNotify 28
#define SelectionClear 29
#define SelectionRequest 30
#define SelectionNotify 31
#define ColormapNotify 32
#define ClientMessage 33
#define MappingNotify 34

/*
** X11 event mask compatibility defines
*/
#define NoEventMask 0L
#define KeyPressMask (1L << 0)
#define KeyReleaseMask (1L << 1)
#define ButtonPressMask (1L << 2)
#define ButtonReleaseMask (1L << 3)
#define EnterWindowMask (1L << 4)
#define LeaveWindowMask (1L << 5)
#define PointerMotionMask (1L << 6)
#define PointerMotionHintMask (1L << 7)
#define Button1MotionMask (1L << 8)
#define Button2MotionMask (1L << 9)
#define Button3MotionMask (1L << 10)
#define Button4MotionMask (1L << 11)
#define Button5MotionMask (1L << 12)
#define ButtonMotionMask (1L << 13)
#define KeymapStateMask (1L << 14)
#define ExposureMask (1L << 15)
#define VisibilityChangeMask (1L << 16)
#define StructureNotifyMask (1L << 17)
#define ResizeRedirectMask (1L << 18)
#define SubstructureNotifyMask (1L << 19)
#define SubstructureRedirectMask (1L << 20)
#define FocusChangeMask (1L << 21)
#define PropertyChangeMask (1L << 22)
#define ColormapChangeMask (1L << 23)
#define OwnerGrabButtonMask (1L << 24)

#endif /* MLX_H */
