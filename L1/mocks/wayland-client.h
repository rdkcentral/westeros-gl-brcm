/*
 * Mock Wayland Client Header for L1 Testing
 * Minimal wayland-client.h definitions
 */

#ifndef _WAYLAND_CLIENT_H
#define _WAYLAND_CLIENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_surface;
struct wl_buffer;
struct wl_callback;
struct wl_shm;
struct wl_shm_pool;

// Minimal proxy structure
struct wl_proxy;

// Event queue
struct wl_event_queue;

// Display functions (minimal mocks)
static inline struct wl_display* wl_display_connect(const char *name) {
    (void)name;
    return nullptr;
}

static inline void wl_display_disconnect(struct wl_display *display) {
    (void)display;
}

static inline int wl_display_roundtrip(struct wl_display *display) {
    (void)display;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // _WAYLAND_CLIENT_H
