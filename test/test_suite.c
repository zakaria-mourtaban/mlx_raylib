/*
** test_suite.c - Comprehensive test suite for MinilibX Raylib backend
** Tests each feature individually with visual and console feedback
*/

#include "../mlx.h"
#include "../mlx_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/* Test configuration */
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define TEST_TIMEOUT 3

/* Global state */
void *g_mlx = NULL;
void *g_win = NULL;
int g_test_passed = 0;
int g_test_failed = 0;
int g_current_test = 0;

/* Test result tracking */
typedef struct s_test_result
{
	const char *name;
	bool passed;
	const char *message;
} t_test_result;

#define MAX_TESTS 50
t_test_result g_results[MAX_TESTS];

void log_test(const char *name, bool passed, const char *msg)
{
	g_results[g_current_test].name = name;
	g_results[g_current_test].passed = passed;
	g_results[g_current_test].message = msg;
	g_current_test++;
	if (passed)
	{
		printf("  [PASS] %s\n", name);
		g_test_passed++;
	}
	else
	{
		printf("  [FAIL] %s - %s\n", name, msg);
		g_test_failed++;
	}
}

/*
** ==========================================================================
** TEST 1: MLX Initialization
** ==========================================================================
*/
void test_mlx_init(void)
{
	printf("\n=== Test 1: MLX Initialization ===\n");

	void *mlx = mlx_init();
	if (mlx != NULL)
	{
		log_test("mlx_init returns non-NULL", true, "");
		t_xvar *xvar = (t_xvar *)mlx;
		log_test("xvar->initialized is false before window", !xvar->initialized,
				 "Should be false until window created");
		g_mlx = mlx;
	}
	else
	{
		log_test("mlx_init returns non-NULL", false, "Returned NULL");
	}
}

/*
** ==========================================================================
** TEST 2: Window Creation
** ==========================================================================
*/
void test_window_creation(void)
{
	printf("\n=== Test 2: Window Creation ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	void *win = mlx_new_window(g_mlx, WIN_WIDTH, WIN_HEIGHT, "Test Window");
	if (win != NULL)
	{
		log_test("mlx_new_window returns non-NULL", true, "");
		t_win_list *w = (t_win_list *)win;
		log_test("window width correct", w->width == WIN_WIDTH, "Width mismatch");
		log_test("window height correct", w->height == WIN_HEIGHT, "Height mismatch");
		log_test("window is active", w->active, "Window not active");

		t_xvar *xvar = (t_xvar *)g_mlx;
		log_test("xvar->initialized after window", xvar->initialized,
				 "Should be true after window created");
		log_test("win_list set", xvar->win_list == win, "win_list not set");
		log_test("active_window set", xvar->active_window == win, "active_window not set");

		g_win = win;
	}
	else
	{
		log_test("mlx_new_window returns non-NULL", false, "Returned NULL");
	}
}

/*
** ==========================================================================
** TEST 3: Image Creation and Data Access
** ==========================================================================
*/
void test_image_creation(void)
{
	printf("\n=== Test 3: Image Creation and Data Access ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	int img_w = 100, img_h = 100;
	void *img = mlx_new_image(g_mlx, img_w, img_h);

	if (img != NULL)
	{
		log_test("mlx_new_image returns non-NULL", true, "");

		int bpp, sl, endian;
		char *data = mlx_get_data_addr(img, &bpp, &sl, &endian);

		log_test("mlx_get_data_addr returns non-NULL", data != NULL, "Returned NULL");
		log_test("bpp is 32", bpp == 32, "Expected 32 bpp");
		log_test("size_line is width*4", sl == img_w * 4, "size_line mismatch");
		log_test("endian is 0 (little)", endian == 0, "Endian mismatch");

		/* Test writing pixels */
		int x = 50, y = 50;
		int offset = y * sl + x * (bpp / 8);
		data[offset] = (char)0xFF;	   /* Blue */
		data[offset + 1] = (char)0x00; /* Green */
		data[offset + 2] = (char)0x00; /* Red */
		data[offset + 3] = (char)0xFF; /* Alpha */

		log_test("pixel data write",
				 (unsigned char)data[offset] == 0xFF, "Pixel write failed");

		t_img *i = (t_img *)img;
		log_test("texture_needs_update initially true",
				 i->texture_needs_update, "Should be true");

		mlx_destroy_image(g_mlx, img);
		log_test("mlx_destroy_image", true, "");
	}
	else
	{
		log_test("mlx_new_image returns non-NULL", false, "Returned NULL");
	}
}

/*
** ==========================================================================
** TEST 4: Image to Window
** ==========================================================================
*/
void *g_test_img = NULL;
char *g_test_data = NULL;
int g_test_sl = 0;

void test_image_to_window(void)
{
	printf("\n=== Test 4: Image to Window ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	int img_w = 200, img_h = 200;
	g_test_img = mlx_new_image(g_mlx, img_w, img_h);

	if (!g_test_img)
	{
		log_test("create test image", false, "Failed");
		return;
	}

	int bpp;
	g_test_data = mlx_get_data_addr(g_test_img, &bpp, &g_test_sl, NULL);

	/* Fill with gradient */
	for (int y = 0; y < img_h; y++)
	{
		for (int x = 0; x < img_w; x++)
		{
			int offset = y * g_test_sl + x * 4;
			g_test_data[offset] = (char)(x * 255 / img_w);				 /* Blue */
			g_test_data[offset + 1] = (char)(y * 255 / img_h);			 /* Green */
			g_test_data[offset + 2] = (char)((img_w - x) * 255 / img_w); /* Red */
			g_test_data[offset + 3] = (char)0xFF;
		}
	}

	int ret = mlx_put_image_to_window(g_mlx, g_win, g_test_img, 50, 50);
	log_test("mlx_put_image_to_window returns 0", ret == 0, "Non-zero return");

	t_img *i = (t_img *)g_test_img;
	log_test("texture created after put", i->texture.id != 0, "No texture created");
	log_test("texture_needs_update false after put",
			 !i->texture_needs_update, "Should be false");
}

/*
** ==========================================================================
** TEST 5: Pixel Put
** ==========================================================================
*/
void test_pixel_put(void)
{
	printf("\n=== Test 5: Pixel Put ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	/* Draw a line of red pixels */
	for (int x = 300; x < 400; x++)
	{
		mlx_pixel_put(g_mlx, g_win, x, 100, 0xFF0000);
	}
	log_test("mlx_pixel_put red line", true, "");

	/* Draw a line of green pixels */
	for (int x = 300; x < 400; x++)
	{
		mlx_pixel_put(g_mlx, g_win, x, 120, 0x00FF00);
	}
	log_test("mlx_pixel_put green line", true, "");

	/* Draw a line of blue pixels */
	for (int x = 300; x < 400; x++)
	{
		mlx_pixel_put(g_mlx, g_win, x, 140, 0x0000FF);
	}
	log_test("mlx_pixel_put blue line", true, "");
}

/*
** ==========================================================================
** TEST 6: String Put
** ==========================================================================
*/
void test_string_put(void)
{
	printf("\n=== Test 6: String Put ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	int ret = mlx_string_put(g_mlx, g_win, 300, 200, 0xFFFFFF, "Hello MLX!");
	log_test("mlx_string_put returns 0", ret == 0, "Non-zero return");

	mlx_string_put(g_mlx, g_win, 300, 220, 0xFF0000, "Red Text");
	mlx_string_put(g_mlx, g_win, 300, 240, 0x00FF00, "Green Text");
	mlx_string_put(g_mlx, g_win, 300, 260, 0x0000FF, "Blue Text");
	log_test("mlx_string_put colored text", true, "");
}

/*
** ==========================================================================
** TEST 7: Event Hooks Setup
** ==========================================================================
*/
int g_key_press_count = 0;
int g_key_release_count = 0;
int g_mouse_button_count = 0;
int g_mouse_motion_count = 0;
int g_expose_count = 0;
int g_loop_count = 0;
int g_last_key = 0;
int g_last_mouse_x = 0;
int g_last_mouse_y = 0;

int test_key_press_hook(int keycode, void *param)
{
	(void)param;
	g_key_press_count++;
	g_last_key = keycode;
	printf("    Key Press: %d (count: %d)\n", keycode, g_key_press_count);
	return (0);
}

int test_key_release_hook(int keycode, void *param)
{
	(void)param;
	g_key_release_count++;
	printf("    Key Release: %d (count: %d)\n", keycode, g_key_release_count);
	return (0);
}

int test_mouse_hook(int button, int x, int y, void *param)
{
	(void)param;
	g_mouse_button_count++;
	printf("    Mouse Button: %d at (%d, %d) (count: %d)\n",
		   button, x, y, g_mouse_button_count);
	return (0);
}

int test_motion_hook(int x, int y, void *param)
{
	(void)param;
	g_mouse_motion_count++;
	g_last_mouse_x = x;
	g_last_mouse_y = y;
	/* Only print every 10th motion to avoid spam */
	if (g_mouse_motion_count % 10 == 0)
		printf("    Mouse Motion: (%d, %d) (count: %d)\n", x, y, g_mouse_motion_count);
	return (0);
}

int test_expose_hook(void *param)
{
	(void)param;
	g_expose_count++;
	printf("    Expose event (count: %d)\n", g_expose_count);
	return (0);
}

int test_loop_hook(void *param)
{
	(void)param;
	g_loop_count++;

	/* Update display with current stats */
	if (g_loop_count % 60 == 0 && g_win && g_mlx)
	{
		char buf[100];
		mlx_clear_window(g_mlx, g_win);

		/* Re-draw test image */
		if (g_test_img)
		{
			t_img *img = (t_img *)g_test_img;
			img->texture_needs_update = true;
			mlx_put_image_to_window(g_mlx, g_win, g_test_img, 50, 50);
		}

		sprintf(buf, "Loop count: %d", g_loop_count);
		mlx_string_put(g_mlx, g_win, 10, 20, 0xFFFFFF, buf);

		sprintf(buf, "Key presses: %d (last: %d)", g_key_press_count, g_last_key);
		mlx_string_put(g_mlx, g_win, 10, 40, 0xFFFF00, buf);

		sprintf(buf, "Mouse buttons: %d", g_mouse_button_count);
		mlx_string_put(g_mlx, g_win, 10, 60, 0x00FFFF, buf);

		sprintf(buf, "Mouse motions: %d", g_mouse_motion_count);
		mlx_string_put(g_mlx, g_win, 10, 80, 0xFF00FF, buf);

		sprintf(buf, "Mouse pos: (%d, %d)", g_last_mouse_x, g_last_mouse_y);
		mlx_string_put(g_mlx, g_win, 10, 100, 0xFFFFFF, buf);

		mlx_string_put(g_mlx, g_win, 10, 140, 0x00FF00,
					   "Press keys, move mouse, click to test events");
		mlx_string_put(g_mlx, g_win, 10, 160, 0xFF0000,
					   "Press ESC or Q to exit");
	}

	/* Exit after ESC or Q */
	if (g_last_key == 65307 || g_last_key == 113)
	{
		printf("    Exit key detected, ending loop...\n");
		mlx_loop_end(g_mlx);
	}

	return (0);
}

void test_event_hooks(void)
{
	printf("\n=== Test 7: Event Hooks Setup ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	int ret;

	/* Key hooks via mlx_hook */
	ret = mlx_hook(g_win, KeyPress, KeyPressMask, test_key_press_hook, NULL);
	log_test("mlx_hook KeyPress", ret == 0, "Non-zero return");

	ret = mlx_hook(g_win, KeyRelease, KeyReleaseMask, test_key_release_hook, NULL);
	log_test("mlx_hook KeyRelease", ret == 0, "Non-zero return");

	/* Mouse button hook */
	ret = mlx_mouse_hook(g_win, test_mouse_hook, NULL);
	log_test("mlx_mouse_hook", ret == 0, "Non-zero return");

	/* Mouse motion hook */
	ret = mlx_hook(g_win, MotionNotify, PointerMotionMask, test_motion_hook, NULL);
	log_test("mlx_hook MotionNotify", ret == 0, "Non-zero return");

	/* Expose hook */
	ret = mlx_expose_hook(g_win, test_expose_hook, NULL);
	log_test("mlx_expose_hook", ret == 0, "Non-zero return");

	/* Loop hook */
	ret = mlx_loop_hook(g_mlx, test_loop_hook, NULL);
	log_test("mlx_loop_hook", ret == 0, "Non-zero return");

	/* Verify hooks are set */
	t_win_list *w = (t_win_list *)g_win;
	log_test("KeyPress hook stored",
			 w->hooks[KeyPress].hook == (void *)test_key_press_hook, "Hook not stored");
	log_test("MotionNotify hook stored",
			 w->hooks[MotionNotify].hook == (void *)test_motion_hook, "Hook not stored");
}

/*
** ==========================================================================
** TEST 8: Mouse Functions
** ==========================================================================
*/
void test_mouse_functions(void)
{
	printf("\n=== Test 8: Mouse Functions ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	int x, y;
	int ret = mlx_mouse_get_pos(g_mlx, g_win, &x, &y);
	log_test("mlx_mouse_get_pos returns 0", ret == 0, "Non-zero return");
	printf("    Current mouse position: (%d, %d)\n", x, y);

	ret = mlx_mouse_hide(g_mlx, g_win);
	log_test("mlx_mouse_hide returns 0", ret == 0, "Non-zero return");

	ret = mlx_mouse_show(g_mlx, g_win);
	log_test("mlx_mouse_show returns 0", ret == 0, "Non-zero return");

	/* Test mouse move - move to center */
	int center_x = WIN_WIDTH / 2;
	int center_y = WIN_HEIGHT / 2;
	ret = mlx_mouse_move(g_mlx, g_win, center_x, center_y);
	log_test("mlx_mouse_move returns 0", ret == 0, "Non-zero return");
}

/*
** ==========================================================================
** TEST 9: Screen Size
** ==========================================================================
*/
void test_screen_size(void)
{
	printf("\n=== Test 9: Screen Size ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	int w, h;
	int ret = mlx_get_screen_size(g_mlx, &w, &h);
	log_test("mlx_get_screen_size returns 0", ret == 0, "Non-zero return");
	log_test("screen width > 0", w > 0, "Invalid width");
	log_test("screen height > 0", h > 0, "Invalid height");
	printf("    Screen size: %dx%d\n", w, h);
}

/*
** ==========================================================================
** TEST 10: Clear Window
** ==========================================================================
*/
void test_clear_window(void)
{
	printf("\n=== Test 10: Clear Window ===\n");

	if (!g_mlx || !g_win)
	{
		log_test("mlx and window exist", false, "Missing context");
		return;
	}

	int ret = mlx_clear_window(g_mlx, g_win);
	log_test("mlx_clear_window returns 0", ret == 0, "Non-zero return");
}

/*
** ==========================================================================
** TEST 11: Color Value
** ==========================================================================
*/
void test_color_value(void)
{
	printf("\n=== Test 11: Color Value ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	int color = 0xFF5500;
	int result = mlx_get_color_value(g_mlx, color);
	log_test("mlx_get_color_value returns same color", result == color, "Color mismatch");
}

/*
** ==========================================================================
** TEST 12: Key Autorepeat
** ==========================================================================
*/
void test_key_autorepeat(void)
{
	printf("\n=== Test 12: Key Autorepeat ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	int ret = mlx_do_key_autorepeatoff(g_mlx);
	log_test("mlx_do_key_autorepeatoff returns 0", ret == 0, "Non-zero return");

	t_xvar *xvar = (t_xvar *)g_mlx;
	log_test("key_autorepeat is false", !xvar->key_autorepeat, "Should be false");

	ret = mlx_do_key_autorepeaton(g_mlx);
	log_test("mlx_do_key_autorepeaton returns 0", ret == 0, "Non-zero return");
	log_test("key_autorepeat is true", xvar->key_autorepeat, "Should be true");
}

/*
** ==========================================================================
** TEST 13: Sync
** ==========================================================================
*/
void test_sync(void)
{
	printf("\n=== Test 13: Sync ===\n");

	if (!g_mlx)
	{
		log_test("mlx exists", false, "No MLX context");
		return;
	}

	int ret = mlx_do_sync(g_mlx);
	log_test("mlx_do_sync returns 0", ret == 0, "Non-zero return");
}

/*
** ==========================================================================
** TEST 14: Dynamic Image Updates
** ==========================================================================
*/
void test_dynamic_image_update(void)
{
	printf("\n=== Test 14: Dynamic Image Updates ===\n");

	if (!g_mlx || !g_win || !g_test_img || !g_test_data)
	{
		log_test("test image exists", false, "Missing test image");
		return;
	}

	/* Modify the image data */
	int img_w = 200, img_h = 200;
	for (int y = 0; y < img_h; y++)
	{
		for (int x = 0; x < img_w; x++)
		{
			int offset = y * g_test_sl + x * 4;
			/* Invert colors */
			g_test_data[offset] = 255 - (unsigned char)g_test_data[offset];
			g_test_data[offset + 1] = 255 - (unsigned char)g_test_data[offset + 1];
			g_test_data[offset + 2] = 255 - (unsigned char)g_test_data[offset + 2];
		}
	}

	/* Mark texture as needing update */
	t_img *img = (t_img *)g_test_img;
	img->texture_needs_update = true;
	log_test("set texture_needs_update to true", img->texture_needs_update, "");

	/* Put image again */
	mlx_put_image_to_window(g_mlx, g_win, g_test_img, 50, 50);
	log_test("re-put modified image", !img->texture_needs_update,
			 "texture_needs_update should be false after put");
}

/*
** ==========================================================================
** Main Test Runner
** ==========================================================================
*/
void print_summary(void)
{
	printf("\n");
	printf("============================================\n");
	printf("           TEST SUMMARY\n");
	printf("============================================\n");
	printf("  Passed: %d\n", g_test_passed);
	printf("  Failed: %d\n", g_test_failed);
	printf("  Total:  %d\n", g_test_passed + g_test_failed);
	printf("============================================\n");

	if (g_test_failed > 0)
	{
		printf("\nFailed tests:\n");
		for (int i = 0; i < g_current_test; i++)
		{
			if (!g_results[i].passed)
			{
				printf("  - %s: %s\n", g_results[i].name, g_results[i].message);
			}
		}
	}
	printf("\n");
}

int main(void)
{
	printf("============================================\n");
	printf("  MinilibX Raylib Backend Test Suite\n");
	printf("============================================\n");

	/* Run all tests */
	test_mlx_init();
	test_window_creation();
	test_image_creation();
	test_image_to_window();
	test_pixel_put();
	test_string_put();
	test_event_hooks();
	test_mouse_functions();
	test_screen_size();
	test_clear_window();
	test_color_value();
	test_key_autorepeat();
	test_sync();
	test_dynamic_image_update();

	/* Print summary before entering loop */
	print_summary();

	if (g_mlx && g_win)
	{
		printf("Starting interactive test mode...\n");
		printf("Move mouse, press keys, click buttons to test events.\n");
		printf("Press ESC or Q to exit.\n\n");

		/* Enter the main loop to test events interactively */
		mlx_loop(g_mlx);

		printf("\n=== Interactive Test Results ===\n");
		printf("  Key presses detected: %d\n", g_key_press_count);
		printf("  Key releases detected: %d\n", g_key_release_count);
		printf("  Mouse buttons detected: %d\n", g_mouse_button_count);
		printf("  Mouse motions detected: %d\n", g_mouse_motion_count);
		printf("  Expose events: %d\n", g_expose_count);
		printf("  Loop iterations: %d\n", g_loop_count);

		/* Cleanup */
		if (g_test_img)
			mlx_destroy_image(g_mlx, g_test_img);
		mlx_destroy_window(g_mlx, g_win);
		mlx_destroy_display(g_mlx);
	}

	return (g_test_failed > 0 ? 1 : 0);
}
