#ifndef RAYXR_INTERNAL_H
#define RAYXR_INTERNAL_H

struct rayxr_context {
        XrInstance instance;
        XrSystemId device;
        XrSession session;
        XrSpace space;
        XrSwapchain swapchains[2];
        XrFrameState frame_state;
        XrView views[2];
        uint32_t eye_width[2];
        uint32_t eye_height[2];

        GLuint fbos[2];
        GLuint depth[2];
        Gluint eye_imgs[2][4];
        uint32_t eye_img_count[2];

        int current_eye;
};

void session_poll(void);

#endif /* RAYXR_INTERNAL_H */
