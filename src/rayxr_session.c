#include "rayxr_internal.h"
#include <openxr/openxr.h>

extern struct rayxr_context ctx;

static int running = 0;

static void session_poll(void)
{
        XrEventDataBuffer ev = {.type = XR_TYPE_EVENT_DATA_BUFFER};

        while (xrPollEvent(ctx.instance, &ev) == XR_SUCCESS) {
                switch (ev.type) {
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                        XrEventDataSessionStateChanged* sc = (XrEventDataSessionStateChanged*)&ev;

                        switch (sc->state) {
                        case XR_SESSION_STATE_READY: {
                                XrSessionBeginInfo bi = {
                                        .type = XR_TYPE_SESSION_BEGIN_INFO,
                                        .primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO
                                };
                                xrBeginSession(ctx.session, &bi);
                                running = 1;
                                break;
                        }
                        case XR_SESSION_STATE_STOPPING:
                                xrEndSession(ctx.session);
                                running = 0;
                                break;

                        case XR_SESSION_STATE_EXITING:
                        case XR_SESSION_STATE_LOSS_PENDING:
                                running = 0;
                                break;

                        default:
                                break;
                        }
                        break;
                }
                default:
                        break;
                }
                ev.type = XR_TYPE_EVENT_DATA_BUFFER;
        }
}

int rxr_is_running(void) { return running; }
XrSession rxr_session(void) { return ctx.session; }
XrInstance rxr_instance(void) { return ctx.instance; }
