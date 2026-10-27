#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int NEXUS_Error;
#define NEXUS_SUCCESS 0
#define NEXUS_GRAPHICS2D_QUEUED 1
#define NEXUS_ANY_ID 0
#define NEXUS_OFFSCREEN_SURFACE 0
#define NEXUS_SecureVideo_eSecure 1
#define NEXUS_PixelFormat_eA8_B8_G8_R8 0
#define NEXUS_PixelFormat_eR8_G8_B8_A8 1
#define BEGL_BufferFormat_eA8B8G8R8 0
#define BEGL_BufferFormat_eR8G8B8A8 1
#define NEXUS_PLATFORM_VERSION_MAJOR 16
#define NXCLIENT_MAX_NAME 64
#define NXCLIENT_SECURE_GRAPHICS_HEAP 1
#define BKNI_INFINITE 0xffffffffu

typedef void *NEXUS_SurfaceHandle;
typedef void *NEXUS_SurfaceClientHandle;
typedef void *NEXUS_Graphics2DHandle;
typedef void *NXPL_PlatformHandle;
typedef void *BKNI_EventHandle;

typedef struct { char name[NXCLIENT_MAX_NAME]; } NxClient_JoinSettings;
typedef struct { int surfaceClient; } NxClient_AllocSettings;
typedef struct { struct { int id; } surfaceClient[1]; } NxClient_AllocResults;
typedef struct { bool secure; } NxClient_DisplaySettings;
typedef struct { int secure; } NEXUS_Graphics2DOpenSettings;
typedef struct { struct { void (*callback)(void *, int); void *context; int param; } checkpointCallback; } NEXUS_Graphics2DSettings;
typedef struct { int width; int height; } NEXUS_Framebuffer;
typedef struct { NEXUS_Framebuffer framebuffer; } NEXUS_DisplayStatus;
typedef struct { NEXUS_DisplayStatus display; } NEXUS_SurfaceClientStatus;
typedef struct { struct { void (*callback)(void *, int); void *context; int param; } displayStatusChanged; } NEXUS_SurfaceClientSettings;
typedef struct { int width; int height; } NEXUS_SurfaceStatus;
typedef struct { bool graphicsv3d; } NEXUS_Compatibility;
typedef struct { int width; int height; int pixelFormat; int mipLevel; int heap; int alignment; NEXUS_Compatibility compatibility; } NEXUS_SurfaceCreateSettings;
typedef struct { int heap[2]; } NEXUS_ClientConfiguration;
typedef struct { NEXUS_SurfaceHandle surface; } NEXUS_BlitSurface;
typedef struct { NEXUS_BlitSurface source; NEXUS_BlitSurface output; } NEXUS_Graphics2DBlitSettings;
typedef struct { int width; int height; int format; bool secure; } BEGL_PixmapInfoEXT;
typedef struct { int width; int height; int format; } BEGL_PixmapInfo;
typedef struct { int x; int y; int width; int height; bool stretch; int clientID; int zOrder; } NXPL_NativeWindowInfo;

void NxClient_GetDefaultJoinSettings(NxClient_JoinSettings *settings);
NEXUS_Error NxClient_Join(NxClient_JoinSettings *settings);
void NxClient_GetDefaultAllocSettings(NxClient_AllocSettings *settings);
NEXUS_Error NxClient_Alloc(NxClient_AllocSettings *settings, NxClient_AllocResults *results);
void NxClient_Free(NxClient_AllocResults *results);
void NxClient_Uninit(void);
void NxClient_GetDisplaySettings(NxClient_DisplaySettings *settings);
NEXUS_Error NxClient_SetDisplaySettings(NxClient_DisplaySettings *settings);
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire(int id);
void NEXUS_SurfaceClient_Release(NEXUS_SurfaceClientHandle client);
void NEXUS_SurfaceClient_GetSettings(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientSettings *settings);
NEXUS_Error NEXUS_SurfaceClient_SetSettings(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientSettings *settings);
NEXUS_Error NEXUS_SurfaceClient_GetStatus(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientStatus *status);
NEXUS_Error NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle *handle, void *display);
void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle handle);
NEXUS_Error BKNI_CreateEvent(BKNI_EventHandle *event);
void BKNI_DestroyEvent(BKNI_EventHandle event);
void BKNI_SetEvent(BKNI_EventHandle event);
NEXUS_Error BKNI_WaitForEvent(BKNI_EventHandle event, unsigned int timeout);
void NEXUS_Graphics2D_GetDefaultOpenSettings(NEXUS_Graphics2DOpenSettings *settings);
NEXUS_Graphics2DHandle NEXUS_Graphics2D_Open(int id, NEXUS_Graphics2DOpenSettings *settings);
void NEXUS_Graphics2D_GetSettings(NEXUS_Graphics2DHandle handle, NEXUS_Graphics2DSettings *settings);
NEXUS_Error NEXUS_Graphics2D_SetSettings(NEXUS_Graphics2DHandle handle, NEXUS_Graphics2DSettings *settings);
void NEXUS_Graphics2D_Close(NEXUS_Graphics2DHandle handle);
void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *settings);
void NEXUS_Platform_GetClientConfiguration(NEXUS_ClientConfiguration *config);
int NEXUS_Platform_GetFramebufferHeap(int surfaceType);
NEXUS_SurfaceHandle NEXUS_Surface_Create(NEXUS_SurfaceCreateSettings *settings);
NEXUS_Error NEXUS_Surface_GetStatus(NEXUS_SurfaceHandle surface, NEXUS_SurfaceStatus *status);
void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle surface);
void NXPL_GetDefaultPixmapInfoEXT(BEGL_PixmapInfoEXT *info);
bool NXPL_CreateCompatiblePixmapEXT(NXPL_PlatformHandle handle, void **pixmap, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfoEXT *info);
bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle handle, void **pixmap, NEXUS_SurfaceHandle *surface, BEGL_PixmapInfo *info);
void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle handle, void *pixmap);
void *NXPL_CreateNativeWindow(NXPL_NativeWindowInfo *info);
void NXPL_DestroyNativeWindow(void *window);
void NEXUS_Graphics2D_GetDefaultBlitSettings(NEXUS_Graphics2DBlitSettings *settings);
NEXUS_Error NEXUS_Graphics2D_Blit(NEXUS_Graphics2DHandle handle, NEXUS_Graphics2DBlitSettings *settings);
NEXUS_Error NEXUS_Graphics2D_Checkpoint(NEXUS_Graphics2DHandle handle, void *checkpoint);
void NEXUS_GetVideoDecoderCapabilities(void *capabilities);

#ifdef __cplusplus
}
#endif

typedef struct { struct { int secure; } memory[1]; } NEXUS_VideoDecoderCapabilities;
