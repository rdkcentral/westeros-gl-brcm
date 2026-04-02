/*
 * Mock Westeros Renderer Header for L1 Testing
 * This header provides the renderer interface used by westeros-render-nexus.cpp
 */

#ifndef _WESTEROS_RENDER_H
#define _WESTEROS_RENDER_H

#include <stdint.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct wl_resource;
struct wl_display;

// Rectangle structure
typedef struct _WstRect
{
   int x;
   int y;
   int width;
   int height;
} WstRect;

// Render surface (opaque to compositor)
typedef struct _WstRenderSurface WstRenderSurface;

// Renderer structure
typedef struct _WstRenderer
{
   void *renderer;  // Platform-specific renderer (WstRendererNX)
   void *displayName;
   int outputWidth;
   int outputHeight;
   struct wl_display *display;
   float matrix[16];  // Transformation matrix
   float alpha;       // Global alpha value
   bool needHolePunch;  // Flag for hole punching
   
   // Callback function pointers
   void (*outputHandleGeometry)(void *data, int32_t x, int32_t y,
                                int32_t physical_width, int32_t physical_height,
                                int32_t subpixel, const char *make,
                                const char *model, int32_t transform);
   void (*outputHandleMode)(void *data, uint32_t flags, int32_t width,
                           int32_t height, int32_t refresh);
   void (*outputHandleDone)(void *data);
   void (*outputHandleScale)(void *data, int32_t scale);
   
   // Renderer lifecycle
   void (*renderTerm)(struct _WstRenderer *renderer);
   
   // Scene management
   void (*updateScene)(struct _WstRenderer *renderer);
   void (*delegateUpdateScene)(struct _WstRenderer *renderer, std::vector<WstRect> &rects);
   
   // Surface lifecycle
   WstRenderSurface* (*surfaceCreate)(struct _WstRenderer *renderer);
   void (*surfaceDestroy)(struct _WstRenderer *renderer, WstRenderSurface *surface);
   void (*surfaceCommit)(struct _WstRenderer *renderer, WstRenderSurface *surface, 
                        struct wl_resource *resource);
   
   // Surface properties
   void (*surfaceSetVisible)(struct _WstRenderer *renderer, WstRenderSurface *surface, bool visible);
   bool (*surfaceGetVisible)(struct _WstRenderer *renderer, WstRenderSurface *surface, bool *visible);
   
   void (*surfaceSetGeometry)(struct _WstRenderer *renderer, WstRenderSurface *surface, 
                             int x, int y, int width, int height);
   void (*surfaceGetGeometry)(struct _WstRenderer *renderer, WstRenderSurface *surface, 
                             int *x, int *y, int *width, int *height);
   
   void (*surfaceSetOpacity)(struct _WstRenderer *renderer, WstRenderSurface *surface, float opacity);
   void (*surfaceGetOpacity)(struct _WstRenderer *renderer, WstRenderSurface *surface, float *opacity);
   
   void (*surfaceSetZOrder)(struct _WstRenderer *renderer, WstRenderSurface *surface, float z);
   void (*surfaceGetZOrder)(struct _WstRenderer *renderer, WstRenderSurface *surface, float *z);
} WstRenderer;

// Renderer factory function (implemented by platform)
extern int renderer_init(WstRenderer *renderer, int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif // _WESTEROS_RENDER_H
