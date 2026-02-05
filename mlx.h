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
** Guarded to avoid conflicts if X11/X.h is also included
*/
#ifndef KeyPress
#define KeyPress 2
#endif
#ifndef KeyRelease
#define KeyRelease 3
#endif
#ifndef ButtonPress
#define ButtonPress 4
#endif
#ifndef ButtonRelease
#define ButtonRelease 5
#endif
#ifndef MotionNotify
#define MotionNotify 6
#endif
#ifndef EnterNotify
#define EnterNotify 7
#endif
#ifndef LeaveNotify
#define LeaveNotify 8
#endif
#ifndef FocusIn
#define FocusIn 9
#endif
#ifndef FocusOut
#define FocusOut 10
#endif
#ifndef KeymapNotify
#define KeymapNotify 11
#endif
#ifndef Expose
#define Expose 12
#endif
#ifndef GraphicsExpose
#define GraphicsExpose 13
#endif
#ifndef NoExpose
#define NoExpose 14
#endif
#ifndef VisibilityNotify
#define VisibilityNotify 15
#endif
#ifndef CreateNotify
#define CreateNotify 16
#endif
#ifndef DestroyNotify
#define DestroyNotify 17
#endif
#ifndef UnmapNotify
#define UnmapNotify 18
#endif
#ifndef MapNotify
#define MapNotify 19
#endif
#ifndef MapRequest
#define MapRequest 20
#endif
#ifndef ReparentNotify
#define ReparentNotify 21
#endif
#ifndef ConfigureNotify
#define ConfigureNotify 22
#endif
#ifndef ConfigureRequest
#define ConfigureRequest 23
#endif
#ifndef GravityNotify
#define GravityNotify 24
#endif
#ifndef ResizeRequest
#define ResizeRequest 25
#endif
#ifndef CirculateNotify
#define CirculateNotify 26
#endif
#ifndef CirculateRequest
#define CirculateRequest 27
#endif
#ifndef PropertyNotify
#define PropertyNotify 28
#endif
#ifndef SelectionClear
#define SelectionClear 29
#endif
#ifndef SelectionRequest
#define SelectionRequest 30
#endif
#ifndef SelectionNotify
#define SelectionNotify 31
#endif
#ifndef ColormapNotify
#define ColormapNotify 32
#endif
#ifndef ClientMessage
#define ClientMessage 33
#endif
#ifndef MappingNotify
#define MappingNotify 34
#endif

/*
** X11 event mask compatibility defines
** Guarded to avoid conflicts if X11/X.h is also included
*/
#ifndef NoEventMask
#define NoEventMask 0L
#endif
#ifndef KeyPressMask
#define KeyPressMask (1L << 0)
#endif
#ifndef KeyReleaseMask
#define KeyReleaseMask (1L << 1)
#endif
#ifndef ButtonPressMask
#define ButtonPressMask (1L << 2)
#endif
#ifndef ButtonReleaseMask
#define ButtonReleaseMask (1L << 3)
#endif
#ifndef EnterWindowMask
#define EnterWindowMask (1L << 4)
#endif
#ifndef LeaveWindowMask
#define LeaveWindowMask (1L << 5)
#endif
#ifndef PointerMotionMask
#define PointerMotionMask (1L << 6)
#endif
#ifndef PointerMotionHintMask
#define PointerMotionHintMask (1L << 7)
#endif
#ifndef Button1MotionMask
#define Button1MotionMask (1L << 8)
#endif
#ifndef Button2MotionMask
#define Button2MotionMask (1L << 9)
#endif
#ifndef Button3MotionMask
#define Button3MotionMask (1L << 10)
#endif
#ifndef Button4MotionMask
#define Button4MotionMask (1L << 11)
#endif
#ifndef Button5MotionMask
#define Button5MotionMask (1L << 12)
#endif
#ifndef ButtonMotionMask
#define ButtonMotionMask (1L << 13)
#endif
#ifndef KeymapStateMask
#define KeymapStateMask (1L << 14)
#endif
#ifndef ExposureMask
#define ExposureMask (1L << 15)
#endif
#ifndef VisibilityChangeMask
#define VisibilityChangeMask (1L << 16)
#endif
#ifndef StructureNotifyMask
#define StructureNotifyMask (1L << 17)
#endif
#ifndef ResizeRedirectMask
#define ResizeRedirectMask (1L << 18)
#endif
#ifndef SubstructureNotifyMask
#define SubstructureNotifyMask (1L << 19)
#endif
#ifndef SubstructureRedirectMask
#define SubstructureRedirectMask (1L << 20)
#endif
#ifndef FocusChangeMask
#define FocusChangeMask (1L << 21)
#endif
#ifndef PropertyChangeMask
#define PropertyChangeMask (1L << 22)
#endif
#ifndef ColormapChangeMask
#define ColormapChangeMask (1L << 23)
#endif
#ifndef OwnerGrabButtonMask
#define OwnerGrabButtonMask (1L << 24)
#endif

#endif /* MLX_H */
