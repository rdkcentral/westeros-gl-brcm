/*
 * Mock Simple Buffer Protocol Header for L1 Testing
 * Minimal westeros-simplebuffer.h definitions
 */

#ifndef _WESTEROS_SIMPLEBUFFER_H
#define _WESTEROS_SIMPLEBUFFER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct wl_resource;

// Mock simple buffer structures
typedef struct _WstSBBuffer
{
    int width;
    int height;
    int format;
    void *data;
} WstSBBuffer;

// Simple buffer functions
WstSBBuffer* WstSBBufferGet(struct wl_resource *resource);
void* WstSBBufferGetBuffer(WstSBBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // _WESTEROS_SIMPLEBUFFER_H
