/* Minimal test with direct raylib calls for debugging */
#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static int frame = 0;

void update_draw_frame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(10 + (frame % 400), 10 + (frame % 300), 100, 100, RED);
    DrawText("MLX Raylib WASM Test", 10, 10, 30, BLACK);
    DrawFPS(10, 400);
    EndDrawing();
    frame++;
}

int main(void)
{
    InitWindow(800, 600, "Direct Raylib Test");
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update_draw_frame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
        update_draw_frame();
    CloseWindow();
#endif
    return 0;
}
