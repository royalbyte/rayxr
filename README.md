# rayxr
 
A minimal OpenXR wrapper for [raylib](https://www.raylib.com/).  
Write VR applications using raylib's drawing API without dealing with OpenXR directly.
 
---
 
## What it is
 
rayxr handles the OpenXR boilerplate — instance, session, swapchains, frame loop, pose tracking — and exposes 12 functions so you can draw in VR the same way you draw on a regular screen with raylib.
 
It is not a framework. It does not abstract input, controllers, overlays or layers beyond the bare minimum for stereo rendering. If you need those, you have direct access to the underlying `XrSession` and `XrInstance` handles.
 
---
 
## Dependencies
 
- [raylib](https://github.com/raysan5/raylib) >= 5.0
- [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK) >= 1.0
- OpenGL (via GLX, X11)
- A running OpenXR runtime (e.g. [Monado](https://monado.dev/), SteamVR, Oculus)
 
---
 
## How to compile
 
Build rayxr into an object file and link it with your project:
 
```sh
# compile rayxr
gcc -c src/rayxr_core.c src/rayxr_session.c src/rayxr_render.c src/rayxr_pose.c \
    -Iinclude -I/usr/include/openxr \
    -o rayxr.o
 
# compile and link your project
gcc your_app.c rayxr.o \
    -Iinclude \
    -lraylib -lopenxr_loader -lGL -lX11 \
    -o your_app
```
 
Or add the sources directly to your build system — there is no build system requirement.
 
---
 
## Usage
 
```c
#include <raylib.h>
#include <openxr/openxr.h>
#include <rayxr/rayxr.h>
 
int main(void)
{
    /* raylib window must be created before rxr_init */
    InitWindow(800, 600, "my vr app");
 
    if (rxr_init())
        return 1;
 
    while (!WindowShouldClose() && rxr_is_running()) {
        if (rxr_begin_frame())
            continue;
 
        Camera3D cam = rxr_get_head_camera();
 
        rxr_begin_eye(0);
            ClearBackground(RAYWHITE);
            BeginMode3D(cam);
                /* draw your scene */
            EndMode3D();
        rxr_end_eye();
 
        rxr_begin_eye(1);
            ClearBackground(RAYWHITE);
            BeginMode3D(cam);
                /* draw your scene */
            EndMode3D();
        rxr_end_eye();
 
        rxr_end_frame();
    }
 
    rxr_shutdown();
    CloseWindow();
    return 0;
}
```
 
Include order matters — raylib and openxr must be included before rayxr.h.
 
---
 
## API
 
| Function | Description |
|---|---|
| `rxr_init()` | Initialize OpenXR. Call after `InitWindow()`. Returns 0 on success. |
| `rxr_shutdown()` | Destroy all OpenXR resources. Call before `CloseWindow()`. |
| `rxr_begin_frame()` | Wait for frame, begin frame, locate views. Returns 1 if not ready. |
| `rxr_end_frame()` | Submit rendered eyes to the OpenXR runtime. |
| `rxr_begin_eye(int eye)` | Bind framebuffer and viewport for eye 0 (left) or 1 (right). |
| `rxr_end_eye()` | Release swapchain image for current eye. |
| `rxr_get_head_camera()` | Return a raylib `Camera3D` from current head pose. |
| `rxr_get_head_pose()` | Return raw `XrPosef` for current head pose. |
| `rxr_is_running()` | Returns 1 while the XR session is active. |
| `rxr_session()` | Return the underlying `XrSession` handle. |
| `rxr_instance()` | Return the underlying `XrInstance` handle. |
 
---
 
## Known limitations
 
- **Linux/X11 only.** The OpenGL binding uses GLX (`XR_KHR_opengl_enable`). Windows (WGL) and Wayland are not supported.
- **No per-eye camera.** `rxr_get_head_camera()` returns a single camera from the left eye pose. For physically accurate stereo the two eyes have slightly different positions and FOVs — raylib's `Camera3D` does not model this. For most use cases the difference is imperceptible.
- **No depth submission.** The frame is submitted as a color-only composition layer. Some runtimes use depth for reprojection — this is not supported.
- **No controller input.** rayxr does not wrap OpenXR input. Use `rxr_session()` and the OpenXR input API directly if you need it.
- **Static swapchain images.** The number of swapchain images is assumed to be 4 or fewer. This holds for every known runtime but is not guaranteed by the spec.
