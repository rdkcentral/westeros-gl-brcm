#pragma once

#include "nexus_mock.h"

#ifdef __cplusplus
extern "C" {
#endif

struct WstGLMockSurface
{
   int width;
   int height;
};

void WstGLMockReset(void);
void WstGLMockSetDisplaySize(int width, int height);
void WstGLMockSetInputSurfaceStatus(struct WstGLMockSurface *surface, int width, int height);
void WstGLMockTriggerDisplayChange(int width, int height);
int WstGLMockGetBlitCount(void);
int WstGLMockGetPixmapCreateCount(void);
int WstGLMockGetPixmapDestroyCount(void);
int WstGLMockGetWindowCreateCount(void);
int WstGLMockGetWindowDestroyCount(void);
void WstGLMockFailNextAlloc(void);
void WstGLMockFailNextSurfaceAcquire(void);
void WstGLMockFailNextGraphicsOpen(void);

#ifdef __cplusplus
}
#endif
