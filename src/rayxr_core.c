#include "rayxr_internal.h"
#include <rayxr/rayxr.h>
#include <openxr/openxr.h>
#include <string.h>

#define RXR_CHECK(x) if ((x) != XR_SUCCESS) return false

static struct rayxr_context ctx = {0};

int rxr_init(void)
{
        XrInstanceCreateInfo instanceInfo = {
                .type = XR_TYPE_INSTANCE_CREATE_INFO,
                .applicationInfo = {
                        .applicationName = "rayxr",
                        .applicationVersion = 1,
                        .engineName = "rayxr",
                        .engineVersion = 1,
                        .apiVersion = XR_CURRENT_API_VERSION
                }
        };

        RXR_CHECK(xrCreateInstance(&instanceInfo, &ctx.instance));

        XrSystemGetInfo systemInfo = {
                .type = XR_TYPE_SYSTEM_GET_INFO,
                .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY                
        };

        RXR_CHECK(xrGetSystem(ctx.instance, &systemInfo, &ctx.device));

        XrGraphicsBindingOpenGLXlibKHR glBinding = {
                .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR,
                .next = NULL,
        };
        
        XrSessionCreateInfo sessionInfo = {
                .type = XR_TYPE_SESSION_CREATE_INFO,
                .next = &glBinding,
                .systemId = ctx.device
        };

        RXR_CHECK(xrCreateSession(ctx.instance, &sessionInfo, &ctx.session));

        const XrReferenceSpaceCreateInfo createInfo = {
                .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
                .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL,
                .poseInReferenceSpace = XR_REFERENCE_SPACE_TYPE_STAGE
        };
        
        RXR_CHECK(xrCreateReferenceSpace(ctx.session, &createInfo, &ctx.space));

        // rage quit

        
        return 0;
}
