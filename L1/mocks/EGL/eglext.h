/*
 * Mock EGL Extensions Header for L1 Testing
 * Prevents system eglext.h from being included
 */

#ifndef __eglext_h_
#define __eglext_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <EGL/egl.h>

/* EGL Extension version */
#define EGLEXT_VERSION 20210101

/* Forward declarations needed by westeros */
struct wl_display;
struct wl_resource;

/* EGL extension types already defined in egl.h:
 * EGLAttrib, EGLClientBuffer, EGLSync, EGLImage, EGLTime
 */

/* Stub definitions for extension compatibility */
#define EGL_EGLEXT_VERSION 20210101
#define EGL_KHR_image_base 1
#define EGL_KHR_image 1
#define EGL_WL_bind_wayland_display 1

/* Wayland extension constants */
#define EGL_WAYLAND_BUFFER_WL 0x31D5
#define EGL_WAYLAND_PLANE_WL 0x31D6
#define EGL_TEXTURE_Y_U_V_WL 0x31D7
#define EGL_TEXTURE_Y_UV_WL 0x31D8
#define EGL_TEXTURE_Y_XUXV_WL 0x31D9
#define EGL_TEXTURE_EXTERNAL_WL 0x31DA
#define EGL_WAYLAND_Y_INVERTED_WL 0x31DB

/* Already defined in egl.h - just for reference:
 * typedef EGLBoolean (*PFNEGLBINDWAYLANDDISPLAYWL)(EGLDisplay dpy, struct wl_display *display);
 * typedef EGLBoolean (*PFNEGLUNBINDWAYLANDDISPLAYWL)(EGLDisplay dpy, struct wl_display *display);
 * typedef EGLBoolean (*PFNEGLQUERYWAYLANDBUFFERWL)(EGLDisplay dpy, struct wl_resource *buffer, EGLint attribute, EGLint *value);
 */

#ifdef __cplusplus
}
#endif

#endif /* __eglext_h_ */
