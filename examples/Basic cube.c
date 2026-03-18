#include <raylib.h>
#include <openxr/openxr.h>
#include <rayxr/rayxr.h>

int main(void)
{
        /* raylib window must be created before rxr_init */
        InitWindow(800, 600, "rayxr - basic cube");

        if (rxr_init()) {
                CloseWindow();
                return 1;
        }

        while (!WindowShouldClose() && rxr_is_running()) {
                if (rxr_begin_frame())
                        continue;

                Camera3D cam = rxr_get_head_camera();

                /* left eye */
                rxr_begin_eye(0);
                        ClearBackground(RAYWHITE);
                        BeginMode3D(cam);
                                DrawCube((Vector3){0.0f, 0.0f, -2.0f}, 1.0f, 1.0f, 1.0f, RED);
                                DrawCubeWires((Vector3){0.0f, 0.0f, -2.0f}, 1.0f, 1.0f, 1.0f, DARKGRAY);
                                DrawGrid(10, 1.0f);
                        EndMode3D();
                rxr_end_eye();

                /* right eye */
                rxr_begin_eye(1);
                        ClearBackground(RAYWHITE);
                        BeginMode3D(cam);
                                DrawCube((Vector3){0.0f, 0.0f, -2.0f}, 1.0f, 1.0f, 1.0f, RED);
                                DrawCubeWires((Vector3){0.0f, 0.0f, -2.0f}, 1.0f, 1.0f, 1.0f, DARKGRAY);
                                DrawGrid(10, 1.0f);
                        EndMode3D();
                rxr_end_eye();

                rxr_end_frame();
        }

        rxr_shutdown();
        CloseWindow();
        return 0;
}
