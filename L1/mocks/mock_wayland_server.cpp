/*
 * Mock Wayland Server APIs for L1 Testing
 * Provides minimal wayland-server functionality for renderer tests
 */

#include <cstdlib>
#include <cstring>
#include "wayland-server.h"

// Mock resource structure
struct wl_resource {
    uint32_t id;
    void *data;
    void *client;
    int ref_count;
};

// Mock display structure  
struct wl_display {
    void *user_data;
};

// Global state
static struct wl_display g_mock_display;

extern "C" {

// Create mock resource
struct wl_resource* mock_create_resource(uint32_t id) {
    struct wl_resource *resource = (struct wl_resource*)malloc(sizeof(struct wl_resource));
    if (resource) {
        resource->id = id;
        resource->data = nullptr;
        resource->client = nullptr;
        resource->ref_count = 1;
    }
    return resource;
}

// Destroy mock resource
void mock_destroy_resource(struct wl_resource *resource) {
    if (resource) {
        free(resource);
    }
}

// Get mock display
struct wl_display* mock_get_display() {
    return &g_mock_display;
}

// wl_resource_get_user_data
void* wl_resource_get_user_data(struct wl_resource *resource) {
    return resource ? resource->data : nullptr;
}

// wl_resource_set_user_data
void wl_resource_set_user_data(struct wl_resource *resource, void *data) {
    if (resource) {
        resource->data = data;
    }
}

// wl_display_get_user_data
void* wl_display_get_user_data(struct wl_display *display) {
    return display ? display->user_data : nullptr;
}

// wl_display_set_user_data
void wl_display_set_user_data(struct wl_display *display, void *data) {
    if (display) {
        display->user_data = data;
    }
}

// wl_shm_buffer mock structure
struct wl_shm_buffer {
    int32_t width;
    int32_t height;
    int32_t stride;
    uint32_t format;
    void *data;
};

static struct wl_shm_buffer g_mock_shm_buffer = {
    1920,  // width
    1080,  // height
    7680,  // stride (1920 * 4 bytes)
    0,     // format (WL_SHM_FORMAT_ARGB8888)
    nullptr  // data
};

static unsigned char g_mock_shm_data[1920 * 1080 * 4];

// wl_shm_buffer_get - returns shm buffer from resource or nullptr
struct wl_shm_buffer* wl_shm_buffer_get(struct wl_resource *resource) {
    (void)resource;
    // For testing, return mock buffer
    return &g_mock_shm_buffer;
}

// wl_shm_buffer_get_width
int32_t wl_shm_buffer_get_width(struct wl_shm_buffer *buffer) {
    return buffer ? buffer->width : 0;
}

// wl_shm_buffer_get_height
int32_t wl_shm_buffer_get_height(struct wl_shm_buffer *buffer) {
    return buffer ? buffer->height : 0;
}

// wl_shm_buffer_get_stride
int32_t wl_shm_buffer_get_stride(struct wl_shm_buffer *buffer) {
    return buffer ? buffer->stride : 0;
}

// wl_shm_buffer_get_format
uint32_t wl_shm_buffer_get_format(struct wl_shm_buffer *buffer) {
    return buffer ? buffer->format : 0;
}

// wl_shm_buffer_get_data
void* wl_shm_buffer_get_data(struct wl_shm_buffer *buffer) {
    return buffer ? g_mock_shm_data : nullptr;
}

// wl_shm_buffer_begin_access
void wl_shm_buffer_begin_access(struct wl_shm_buffer *buffer) {
    (void)buffer;
    // No-op for mock
}

// wl_shm_buffer_end_access
void wl_shm_buffer_end_access(struct wl_shm_buffer *buffer) {
    (void)buffer;
    // No-op for mock
}

} // extern "C"
