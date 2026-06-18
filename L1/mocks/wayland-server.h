/*
 * Mock Wayland Server Header for L1 Testing
 * Minimal wayland-server.h definitions for renderer tests
 */

#ifndef _WAYLAND_SERVER_H
#define _WAYLAND_SERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct wl_resource;
struct wl_display;
struct wl_client;
struct wl_shm_buffer;

// SHM buffer format enum
enum wl_shm_format {
    WL_SHM_FORMAT_ARGB8888 = 0,
    WL_SHM_FORMAT_XRGB8888 = 1,
    WL_SHM_FORMAT_BGRA8888 = 2,
    WL_SHM_FORMAT_BGRX8888 = 3,
    WL_SHM_FORMAT_RGB565 = 4,
    WL_SHM_FORMAT_ARGB4444 = 5
};

// Mock functions (implemented in mock_wayland_server.cpp)
extern struct wl_resource* mock_create_resource(uint32_t id);
extern void mock_destroy_resource(struct wl_resource *resource);
extern struct wl_display* mock_get_display(void);

void* wl_resource_get_user_data(struct wl_resource *resource);
void wl_resource_set_user_data(struct wl_resource *resource, void *data);
void* wl_display_get_user_data(struct wl_display *display);
void wl_display_set_user_data(struct wl_display *display, void *data);

// SHM buffer functions
struct wl_shm_buffer* wl_shm_buffer_get(struct wl_resource *resource);
void wl_shm_buffer_begin_access(struct wl_shm_buffer *buffer);
void wl_shm_buffer_end_access(struct wl_shm_buffer *buffer);
void* wl_shm_buffer_get_data(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_width(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_height(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_stride(struct wl_shm_buffer *buffer);
uint32_t wl_shm_buffer_get_format(struct wl_shm_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // _WAYLAND_SERVER_H
