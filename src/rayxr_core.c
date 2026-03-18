#include "rayxr_internal.h"
#include <rayxr/rayxr.h>
#include <openxr/openxr.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdint.h>
#include <string.h>

#define RXR_CHECK(x) if ((x) != XR_SUCCESS) return false
#define OUTPUT_CAPACITY 16

static struct rayxr_context ctx = {0};
static const char* extensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
};

int rxr_init(void)
{
        XrInstanceCreateInfo inst = {
                .type = XR_TYPE_INSTANCE_CREATE_INFO,
                .enabledExtensionCount = 1,
                .enabledExtensionNames = extensions,
                .applicationInfo = {
                        .applicationName = "rayxr",
                        .applicationVersion = 1,
                        .engineName = "rayxr",
                        .engineVersion = 1,
                        .apiVersion = XR_CURRENT_API_VERSION
                }
        };
        RXR_CHECK(xrCreateInstance(&inst, &ctx.instance));

        XrSystemGetInfo sys = {
                .type = XR_TYPE_SYSTEM_GET_INFO,
                .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY                
        };
        RXR_CHECK(xrGetSystem(ctx.instance, &sys, &ctx.device));

        XrGraphicsBindingOpenGLXlibKHR glBinding = {
                .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR,
                .xDisplay = glXGetCurrentDisplay(),
                .glxContext = glXGetCurrentContext(),
                .glxDrawable = glXGetCurrentDrawable()
        };
        
        XrSessionCreateInfo session = {
                .type = XR_TYPE_SESSION_CREATE_INFO,
                .next = &glBinding,
                .systemId = ctx.device
        };
        RXR_CHECK(xrCreateSession(ctx.instance, &session, &ctx.session));

        XrReferenceSpaceCreateInfo space = {
                .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
                .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL,
                .poseInReferenceSpace = {
                        .orientation = {0.0f, 0.0f, 0.0f, 1.0f},
                        .position = {0.0f, 0.0f, 0.0f}
                }
        };        
        RXR_CHECK(xrCreateReferenceSpace(ctx.session, &space, &ctx.space));

        uint32_t fmt_count = OUTPUT_CAPACITY;
        xrEnumerateSwapchainFormats(ctx.session, 0, &fmt_count, NULL);
        int64_t fmts[OUTPUT_CAPACITY];
        xrEnumerateSwapchainFormats(ctx.session, OUTPUT_CAPACITY, &fmt_count, fmts);
        int64_t fmt = fmts[0];

        XrViewConfigurationView vcv[2] = {
                {.type = XR_TYPE_VIEW_CONFIGURATION_VIEW},
                {.type = XR_TYPE_VIEW_CONFIGURATION_VIEW}
        };
        uint32_t vcv_count = 2;
        xrEnumerateViewConfigurationViews(ctx.instance, ctx.device, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2, &vcv_count, vcv);

        for (int eye = 0; eye < 2; ++eye) {
                XrSwapchainCreateInfo sc = {
                        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
                        .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
                        .format = fmt,
                        .sampleCount = 1,
                        .width = vcv[eye].recommendedImageRectWidth,
                        .height = vcv[eye].recommendedImageRectHeight,
                        .faceCount = 1,
                        .arraySize = 1,
                        .mipCount = 1
                };
                RXR_CHECK(xrCreateSwapchain(ctx.session, &sc, &ctx.swapchains[eye]));
                ctx.eye_width[eye] = vcv[eye].recommendedImageRectWidth;
                ctx.eye_height[eye] = vcv[eye].recommendedImageRectHeight;
        }

        for (int eye = 0; eye < 2; ++eye) {
                uint32_t img_count;
                        xrEnumerateSwapchainImages(ctx.swapchains[eye], 0, &img_count, NULL);

                        XrSwapchainImageOpenGLKHR imgs[img_count];
                        for (uint32_t i = 0; i < img_count; ++i)
                                imgs[i].type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;

                        xrEnumerateSwapchainImages(ctx.swapchains[eye], img_count, &img_count, (XrSwapchainImageBaseHeader*)imgs);
                        glGenFramebuffers(1, &ctx.fbos[eye]);
                        glGenRenderbuffers(1, &ctx.depth[eye]);
                        glBindRenderbuffer(GL_RENDERBUFFER, ctx.depth[eye]);
                        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ctx.eye_width[eye], ctx.eye_height[eye]);

                        ctx.eye_img_count[eye] = img_count;
                        for (uint32_t i = 0; i < img_count; ++i )
                                ctx.eye_imgs[eye][i] = imgs[i].image;
        }
        
      
        return 0;
}

void rxr_shutdown(void)
{
        for (int eye = 0; eye < 2; ++eye)
                xrDestroySwapchain(ctx.swapchains[eye]);

        xrDestroySpace(ctx.space);
        xrDestroySession(ctx.session);
        xrDestroyInstance(ctx.instance);
}
