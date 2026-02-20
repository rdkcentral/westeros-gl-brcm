/*
 * Mock Wayland EGL Header for L1 Testing
 * Minimal wayland-egl.h definitions
 */

#ifndef _WAYLAND_EGL_H
#define _WAYLAND_EGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

// Forward declaration
struct wl_surface;
struct wl_display;

// EGL window structure
struct wl_egl_window {
    int width;
    int height;
    int dx;
    int dy;
    int attached_width;
    int attached_height;
    void *driver_private;
    void (*resize_callback)(struct wl_egl_window *, void *);
    void *private_data;
};

// Mock functions (minimal implementation)
static inline struct wl_egl_window* wl_egl_window_create(struct wl_surface *surface,
                                                          int width, int height) {
    (void)surface;
    struct wl_egl_window *window = (struct wl_egl_window*)malloc(sizeof(struct wl_egl_window));
    if (window) {
        window->width = width;
        window->height = height;
        window->dx = 0;
        window->dy = 0;
        window->attached_width = width;
        window->attached_height = height;
        window->driver_private = nullptr;
        window->resize_callback = nullptr;
        window->private_data = nullptr;
    }
    return window;
}

static inline void wl_egl_window_destroy(struct wl_egl_window *egl_window) {
    if (egl_window) {
        free(egl_window);
    }
}

static inline void wl_egl_window_resize(struct wl_egl_window *egl_window,
                                       int width, int height,
                                       int dx, int dy) {
    if (egl_window) {
        egl_window->width = width;
        egl_window->height = height;
        egl_window->dx = dx;
        egl_window->dy = dy;
    }
}

static inline void wl_egl_window_get_attached_size(struct wl_egl_window *egl_window,
                                                   int *width, int *height) {
    if (egl_window) {
        if (width) *width = egl_window->attached_width;
        if (height) *height = egl_window->attached_height;
    }
}

#ifdef __cplusplus
}
#endif

#endif // _WAYLAND_EGL_H
