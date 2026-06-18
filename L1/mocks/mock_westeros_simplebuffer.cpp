/*
 * Mock Westeros Simple Buffer implementation for L1 Testing
 * Provides mock WstSBBuffer functionality
 */

#include "westeros-simplebuffer.h"
#include <cstdlib>
#include <cstring>

// Mock buffer data
static WstSBBuffer g_mock_sb_buffer = {
    1920,  // width
    1080,  // height
    0,     // format
    nullptr  // data
};

static void* g_mock_device_buffer = (void*)0xABCDEF00;

extern "C" {

// WstSBBufferGet - Get simple buffer from wayland resource
WstSBBuffer* WstSBBufferGet(struct wl_resource *resource) {
    (void)resource;
    // For testing, return mock buffer
    return &g_mock_sb_buffer;
}

// WstSBBufferGetBuffer - Get device buffer from simple buffer
void* WstSBBufferGetBuffer(WstSBBuffer *buffer) {
    (void)buffer;
    // Return mock device buffer handle
    return g_mock_device_buffer;
}

} // extern "C"
