#include "nexus_mock_control.h"

#include <map>
#include <utility>
#include <cstring>

namespace
{
int displayWidth = 1920;
int displayHeight = 1080;
int blitCount = 0;
int pixmapCreateCount = 0;
int pixmapDestroyCount = 0;
int windowCreateCount = 0;
int windowDestroyCount = 0;
bool failAlloc = false;
bool failSurfaceAcquire = false;
bool failGraphicsOpen = false;
std::map<NEXUS_SurfaceHandle, std::pair<int, int>> surfaceSizes;
NEXUS_SurfaceClientSettings clientSettings = {};
int displayClientToken = 1;
int graphicsToken = 2;
int eventToken = 3;
int platformToken = 4;
int pixmapToken = 5;
int pixmapSurfaceToken = 6;
int windowToken = 7;
}

extern "C" {
void WstGLMockReset(void)
{
   displayWidth = 1920;
   displayHeight = 1080;
   blitCount = pixmapCreateCount = pixmapDestroyCount = 0;
   windowCreateCount = windowDestroyCount = 0;
   failAlloc = failSurfaceAcquire = failGraphicsOpen = false;
   surfaceSizes.clear();
   clientSettings = {};
}
void WstGLMockSetDisplaySize(int width, int height) { displayWidth = width; displayHeight = height; }
void WstGLMockSetInputSurfaceStatus(WstGLMockSurface *surface, int width, int height)
{
   surface->width = width;
   surface->height = height;
   surfaceSizes[surface] = {width, height};
}
void WstGLMockTriggerDisplayChange(int width, int height)
{
   displayWidth = width;
   displayHeight = height;
   if (clientSettings.displayStatusChanged.callback)
      clientSettings.displayStatusChanged.callback(clientSettings.displayStatusChanged.context, clientSettings.displayStatusChanged.param);
}
int WstGLMockGetBlitCount(void) { return blitCount; }
int WstGLMockGetPixmapCreateCount(void) { return pixmapCreateCount; }
int WstGLMockGetPixmapDestroyCount(void) { return pixmapDestroyCount; }
int WstGLMockGetWindowCreateCount(void) { return windowCreateCount; }
int WstGLMockGetWindowDestroyCount(void) { return windowDestroyCount; }
void WstGLMockFailNextAlloc(void) { failAlloc = true; }
void WstGLMockFailNextSurfaceAcquire(void) { failSurfaceAcquire = true; }
void WstGLMockFailNextGraphicsOpen(void) { failGraphicsOpen = true; }

void NxClient_GetDefaultJoinSettings(NxClient_JoinSettings *settings) { std::memset(settings, 0, sizeof(*settings)); }
NEXUS_Error NxClient_Join(NxClient_JoinSettings *) { return NEXUS_SUCCESS; }
void NxClient_GetDefaultAllocSettings(NxClient_AllocSettings *settings) { settings->surfaceClient = 1; }
NEXUS_Error NxClient_Alloc(NxClient_AllocSettings *, NxClient_AllocResults *results)
{
   if (failAlloc) { failAlloc = false; return 1; }
   results->surfaceClient[0].id = 1;
   return NEXUS_SUCCESS;
}
void NxClient_Free(NxClient_AllocResults *) {}
void NxClient_Uninit(void) {}
void NxClient_GetDisplaySettings(NxClient_DisplaySettings *settings) { settings->secure = false; }
NEXUS_Error NxClient_SetDisplaySettings(NxClient_DisplaySettings *) { return NEXUS_SUCCESS; }
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire(int) { if (failSurfaceAcquire) { failSurfaceAcquire = false; return nullptr; } return &displayClientToken; }
void NEXUS_SurfaceClient_Release(NEXUS_SurfaceClientHandle) {}
void NEXUS_SurfaceClient_GetSettings(NEXUS_SurfaceClientHandle, NEXUS_SurfaceClientSettings *settings) { *settings = {}; }
NEXUS_Error NEXUS_SurfaceClient_SetSettings(NEXUS_SurfaceClientHandle, NEXUS_SurfaceClientSettings *settings) { clientSettings = *settings; return NEXUS_SUCCESS; }
NEXUS_Error NEXUS_SurfaceClient_GetStatus(NEXUS_SurfaceClientHandle, NEXUS_SurfaceClientStatus *status)
{
   status->display.framebuffer.width = displayWidth;
   status->display.framebuffer.height = displayHeight;
   return NEXUS_SUCCESS;
}
NEXUS_Error NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle *handle, void *) { *handle = &platformToken; return NEXUS_SUCCESS; }
void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle) {}
NEXUS_Error BKNI_CreateEvent(BKNI_EventHandle *event) { *event = &eventToken; return NEXUS_SUCCESS; }
void BKNI_DestroyEvent(BKNI_EventHandle) {}
void BKNI_SetEvent(BKNI_EventHandle) {}
NEXUS_Error BKNI_WaitForEvent(BKNI_EventHandle, unsigned int) { return NEXUS_SUCCESS; }
void NEXUS_Graphics2D_GetDefaultOpenSettings(NEXUS_Graphics2DOpenSettings *settings) { *settings = {}; }
NEXUS_Graphics2DHandle NEXUS_Graphics2D_Open(int, NEXUS_Graphics2DOpenSettings *) { if (failGraphicsOpen) { failGraphicsOpen = false; return nullptr; } return &graphicsToken; }
void NEXUS_Graphics2D_GetSettings(NEXUS_Graphics2DHandle, NEXUS_Graphics2DSettings *settings) { *settings = {}; }
NEXUS_Error NEXUS_Graphics2D_SetSettings(NEXUS_Graphics2DHandle, NEXUS_Graphics2DSettings *) { return NEXUS_SUCCESS; }
void NEXUS_Graphics2D_Close(NEXUS_Graphics2DHandle) {}
void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *settings) { *settings = {}; }
void NEXUS_Platform_GetClientConfiguration(NEXUS_ClientConfiguration *config) { *config = {}; }
int NEXUS_Platform_GetFramebufferHeap(int) { return 0; }
NEXUS_SurfaceHandle NEXUS_Surface_Create(NEXUS_SurfaceCreateSettings *settings)
{
   surfaceSizes[&pixmapSurfaceToken] = {settings->width, settings->height};
   return &pixmapSurfaceToken;
}
NEXUS_Error NEXUS_Surface_GetStatus(NEXUS_SurfaceHandle surface, NEXUS_SurfaceStatus *status)
{
   auto found = surfaceSizes.find(surface);
   if (found != surfaceSizes.end()) { status->width = found->second.first; status->height = found->second.second; }
   else { auto *input = static_cast<WstGLMockSurface *>(surface); status->width = input->width; status->height = input->height; }
   return NEXUS_SUCCESS;
}
void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle surface) { surfaceSizes.erase(surface); ++pixmapDestroyCount; }
void NXPL_GetDefaultPixmapInfoEXT(BEGL_PixmapInfoEXT *info) { *info = {}; }
bool NXPL_CreateCompatiblePixmapEXT(NXPL_PlatformHandle, void **pixmap, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfoEXT *info)
{
   *pixmap = &pixmapToken;
   *surface = &pixmapSurfaceToken;
   surfaceSizes[*surface] = {info->width, info->height};
   ++pixmapCreateCount;
   return true;
}
bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle, void **pixmap, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfo *info)
{
   *pixmap = &pixmapToken;
   *surface = &pixmapSurfaceToken;
   surfaceSizes[*surface] = {info->width, info->height};
   ++pixmapCreateCount;
   return true;
}
void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle, void *) { ++pixmapDestroyCount; }
void *NXPL_CreateNativeWindow(NXPL_NativeWindowInfo *) { ++windowCreateCount; return &windowToken; }
void NXPL_DestroyNativeWindow(void *) { ++windowDestroyCount; }
void NEXUS_Graphics2D_GetDefaultBlitSettings(NEXUS_Graphics2DBlitSettings *settings) { *settings = {}; }
NEXUS_Error NEXUS_Graphics2D_Blit(NEXUS_Graphics2DHandle, NEXUS_Graphics2DBlitSettings *) { ++blitCount; return NEXUS_SUCCESS; }
NEXUS_Error NEXUS_Graphics2D_Checkpoint(NEXUS_Graphics2DHandle, void *) { return NEXUS_GRAPHICS2D_QUEUED; }
void NEXUS_GetVideoDecoderCapabilities(void *capabilities)
{
   auto *caps = static_cast<NEXUS_VideoDecoderCapabilities *>(capabilities);
   caps->memory[0].secure = NEXUS_SecureVideo_eSecure;
}
}
