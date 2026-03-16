#ifndef RAYXR_INTERNAL_H
#define RAYXR_INTERNAL_H

typedef struct {
    XrInstance instance;
    XrSession session;
    XrSpace space;

    XrSwapchain swapchains[2];

} RayXRContext;

#endif /* RAYXR_INTERNAL_H */
