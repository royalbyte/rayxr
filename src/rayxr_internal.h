#ifndef RAYXR_INTERNAL_H
#define RAYXR_INTERNAL_H

struct rayxr_context {
        XrInstance instance;
        XrSystemId device;
        XrSession session;
        XrSpace space;

        XrViewConfigurationView view_config[2];
        XrSwapchain swapchains[2];
};

#endif /* RAYXR_INTERNAL_H */
