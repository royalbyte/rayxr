#include "rayxr_internal.h"
#include <openxr/openxr.h>
#include <raylib.h>
#include <math.h>

extern struct rayxr_context ctx;

XrPosef rxr_get_head_pose(void)
{
        return ctx.views[0].pose;
}

Camera3D rxr_get_head_camera(void)
{
        XrPosef *p = &ctx.views[0].pose;

        float x = p->orientation.x;
        float y = p->orientation.y;
        float z = p->orientation.z;
        float w = p->orientation.w;

        Vector3 forward = {
                2.0f * (x*z + w*y),
                2.0f * (y*z - w*x),
                1.0f - 2.0f * (x*x + y*y)
        };

        Vector3 up = {
                2.0f * (x*y - w*z),
                1.0f - 2.0f * (x*x + z*z),
                2.0f * (y*z + w*x)
        };

        Camera3D cam = {
                .position = {
                        p->position.x,
                        p->position.y,
                        p->position.z
                },
                .target = {
                        p->position.x + forward.x,
                        p->position.y + forward.y,
                        p->position.z + forward.z
                },
                .up         = up,
                .fovy       = 90.0f,
                .projection = CAMERA_PERSPECTIVE
        };
        float fovy = (ctx.views[0].fov.angleUp - ctx.views[0].fov.angleDown) * (180.0f / 3.14159f);

        return cam;
}
