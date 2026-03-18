#include "rayxr_internal.h"
#include <openxr/openxr.h>
#include <GL/gl.h>

extern struct rayxr_context ctx;
extern void session_poll(void);

int rxr_begin_frame(void)
{
        session_poll();

        if (!rxr_is_running())
                return 1;

        ctx.frame_state.type = XR_TYPE_FRAME_STATE;
        xrWaitFrame(ctx.session, NULL, &ctx.frame_state);
        xrBeginFrame(ctx.session, NULL);

        XrViewLocateInfo locate = {
                .type = XR_TYPE_VIEW_LOCATE_INFO,
                .viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                .displayTime = ctx.frame_state.predictedDisplayTime,
                .space = ctx.space
        };

        XrViewState view_state = {.type = XR_TYPE_VIEW_STATE};
        ctx.views[0].type = XR_TYPE_VIEW;
        ctx.views[1].type = XR_TYPE_VIEW;

        uint32_t view_count = 2;
        xrLocateViews(ctx.session, &locate, &view_state, 2, &view_count, ctx.views);

        return 0;
}

int rxr_begin_eye(int eye)
{
        ctx.current_eye = eye;
        uint32_t img_index;
        XrSwapchainImageAcquireInfo acq = {.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        xrAcquireSwapchainImage(ctx.swapchains[eye], &acq, &img_index);

        XrSwapchainImageWaitInfo wait = {
                .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
                .timeout = XR_INFINITE_DURATION
        };
        xrWaitSwapchainImage(ctx.swapchains[eye], &wait);

        glBindFramebuffer(GL_FRAMEBUFFER, ctx.fbos[eye]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx.eye_imgs[eye][img_index], 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ctx.depth[eye]);

        glViewport(0, 0, ctx.eye_width[eye], ctx.eye_height[eye]);

        return 0;
}

void rxr_end_eye(void)
{
        XrSwapchainImageReleaseInfo rel = {.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        xrReleaseSwapchainImage(ctx.swapchains[ctx.current_eye], &rel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int rxr_end_frame(void)
{
        XrCompositionLayerProjectionView proj_views[2] = {
                {
                        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
                        .pose = ctx.views[0].pose,
                        .fov  = ctx.views[0].fov,
                        .subImage = {
                                .swapchain = ctx.swapchains[0],
                                .imageRect = {
                                        .offset = {0, 0},
                                        .extent = {ctx.eye_width[0], ctx.eye_height[0]}
                                }
                        }
                },
                {
                        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
                        .pose = ctx.views[1].pose,
                        .fov  = ctx.views[1].fov,
                        .subImage = {
                                .swapchain = ctx.swapchains[1],
                                .imageRect = {
                                        .offset = {0, 0},
                                        .extent = {ctx.eye_width[1], ctx.eye_height[1]}
                                }
                        }
                }
        };

        XrCompositionLayerProjection layer = {
                .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
                .space = ctx.space,
                .viewCount = 2,
                .views = proj_views
        };

        const XrCompositionLayerBaseHeader *layers[] = {
                (XrCompositionLayerBaseHeader *)&layer
        };

        XrFrameEndInfo end = {
                .type = XR_TYPE_FRAME_END_INFO,
                .displayTime = ctx.frame_state.predictedDisplayTime,
                .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
                .layerCount = 1,
                .layers = layers
        };

        xrEndFrame(ctx.session, &end);
        return 0;
}
