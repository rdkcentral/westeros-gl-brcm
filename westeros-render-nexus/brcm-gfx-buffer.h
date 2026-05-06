#ifndef _WESTEROS_BRCM_GFX_BUFFER_H
#define _WESTEROS_BRCM_GFX_BUFFER_H

#include "nexus_surface.h"
#include <memory>

struct brcm_gfx_buffer;

struct brcm_gfx_buffer *WstBGBInit(struct wl_display *display);
void WstBGBUninit(struct brcm_gfx_buffer *gfxbuf);
std::shared_ptr<NEXUS_Surface> WstBGBBufferGet(struct wl_resource *resource);

#endif
