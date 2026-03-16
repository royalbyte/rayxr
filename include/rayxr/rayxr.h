#ifndef RAYXR_H
#define RAYXR_H

/*
 * Initialize the rayxr runtime.
 *
 * Creates the OpenXR instance, system, session, reference space and swapchains,
 * and prepares the OpenGL binding used by raylib.
 *
 * Must be called after the graphics context is created (e.g. after InitWindow()).
 *
 * Returns true on success, false if initialization failed.
 */
bool rxr_init(void);

/*
 * Shutdown the rayxr runtime.
 *
 * Destroys the OpenXR session, swapchains, reference space and instance,
 * releasing all resources allocated during rxr_init().
 */
void rxr_shutdown(void);

/*
 * Begin a new XR frame.
 *
 * Internally calls xrWaitFrame() and xrBeginFrame().
 * Must be called once per frame before rendering any eye.
 *
 * Returns false if the XR runtime is not ready to render.
 */
bool rxr_begin_frame(void);

/*
 * End the current XR frame.
 *
 * Submits all rendered eye images to the OpenXR runtime using xrEndFrame().
 * Must be called after both eyes have been rendered.
 */
bool rxr_end_frame(void);

/*
 * Begin rendering for a specific eye.
 *
 * Acquires the next swapchain image, binds the framebuffer and sets the
 * viewport for the selected eye.
 *
 * eye should be:
 *   0 -> left eye
 *   1 -> right eye
 *
 * Returns false if rendering cannot start for this eye.
 */
bool rxr_begin_eye(int eye);

/*
 * Finish rendering the current eye.
 *
 * Releases the swapchain image and prepares for the next eye or frame submit.
 */
void rxr_end_eye(void);

/*
 * Get a raylib Camera3D representing the current head pose.
 *
 * The returned camera can be used directly with BeginMode3D().
 * Position and orientation are derived from the current OpenXR view pose.
 */
Camera3D rxr_get_head_camera(void);

/*
 * Get the raw OpenXR head pose.
 *
 * Returns the current headset pose in the reference space used by the session.
 * Useful if the application needs direct access to OpenXR tracking data.
 */
XrPosef rxr_get_head_pose(void);

/*
 * Check if the XR runtime is currently running.
 *
 * Returns true while the OpenXR session is active and frames should be rendered.
 */
bool rxr_is_running(void);

/*
 * Get the underlying OpenXR session handle.
 *
 * Exposed so applications can directly call OpenXR functions when needed.
 */
XrSession rxr_session(void);

/*
 * Get the underlying OpenXR instance handle.
 *
 * Allows advanced users to access OpenXR functionality not wrapped by rayxr.
 */
XrInstance rxr_instance(void);

#endif /* RAYXR_H */
