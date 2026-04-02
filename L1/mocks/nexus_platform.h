/**
 * Mock Nexus platform header for L1 testing
 */
#ifndef NEXUS_PLATFORM_H
#define NEXUS_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Handle types
typedef void* NEXUS_HeapHandle;
typedef void* NEXUS_ClientHandle;
typedef void* NEXUS_SurfaceHandle;
typedef void* NEXUS_SurfaceClientHandle;
typedef void* NEXUS_Graphics2DHandle;
typedef void* BKNI_EventHandle;

// Error codes
typedef int NEXUS_Error;
#define NEXUS_SUCCESS 0
#define NEXUS_UNKNOWN 1
#define NEXUS_GRAPHICS2D_QUEUED 2
#define NEXUS_ANY_ID 0

// Macros
#define NXCLIENT_SECURE_GRAPHICS_HEAP 1
#define NEXUS_OFFSCREEN_SURFACE 0
#define BKNI_INFINITE 0xFFFFFFFF
#define BSTD_UNUSED(x) (void)(x)

// Helper macros
#define BKNI_Memset(ptr, val, size) memset(ptr, val, size)

// Pixel formats
typedef enum {
    NEXUS_PixelFormat_eUnknown = 0,
    NEXUS_PixelFormat_eA8_R8_G8_B8 = 1,
    NEXUS_PixelFormat_eR8_G8_B8_A8 = 2,
    NEXUS_PixelFormat_eA8_B8_G8_R8 = 3,
    NEXUS_PixelFormat_eX8_R8_G8_B8 = 4,
    NEXUS_PixelFormat_eB8_G8_R8_A8 = 5,
    NEXUS_PixelFormat_eB8_G8_R8_X8 = 6,
    NEXUS_PixelFormat_eR5_G6_B5 = 7,
    NEXUS_PixelFormat_eA4_R4_G4_B4 = 8
} NEXUS_PixelFormat;

// Blend factors for composition
typedef enum {
    NEXUS_BlendFactor_eZero = 0,
    NEXUS_BlendFactor_eOne = 1,
    NEXUS_BlendFactor_eSourceColor = 2,
    NEXUS_BlendFactor_eInverseSourceColor = 3,
    NEXUS_BlendFactor_eSourceAlpha = 4,
    NEXUS_BlendFactor_eInverseSourceAlpha = 5,
    NEXUS_BlendFactor_eDestinationColor = 6,
    NEXUS_BlendFactor_eInverseDestinationColor = 7,
    NEXUS_BlendFactor_eDestinationAlpha = 8,
    NEXUS_BlendFactor_eInverseDestinationAlpha = 9,
    NEXUS_BlendFactor_eConstantColor = 10,
    NEXUS_BlendFactor_eInverseConstantColor = 11,
    NEXUS_BlendFactor_eConstantAlpha = 12,
    NEXUS_BlendFactor_eInverseConstantAlpha = 13,
    NEXUS_BlendFactor_eSourceAlphaSaturate = 14
} NEXUS_BlendFactor;

// Rectangle
typedef struct {
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
} NEXUS_Rect;

// Surface structures
typedef struct {
    unsigned width;
    unsigned height;
    NEXUS_PixelFormat pixelFormat;
    void* buffer;
    unsigned pitch;
    NEXUS_HeapHandle heap;
    unsigned alignment;
    unsigned mipLevel;
    struct {
        bool graphicsv3d;
    } compatibility;
} NEXUS_SurfaceCreateSettings;

typedef struct {
    unsigned width;
    unsigned height;
    NEXUS_PixelFormat pixelFormat;
} NEXUS_SurfaceStatus;

typedef struct {
    unsigned width;
    unsigned height;
    void *buffer;      // Memory buffer pointer
    unsigned pitch;    // Bytes per line
} NEXUS_SurfaceMemory;

// Graphics2D structures
typedef struct {
    struct {
        NEXUS_SurfaceHandle surface;
        NEXUS_Rect rect;
    } source;
    struct {
        NEXUS_SurfaceHandle surface;
        NEXUS_Rect rect;
    } output;
} NEXUS_Graphics2DBlitSettings;

typedef struct {
    void (*callback)(void*, int);
    void *context;
    int param;
} NEXUS_CallbackDesc;

typedef struct {
    NEXUS_CallbackDesc checkpointCallback;
} NEXUS_Graphics2DSettings;

typedef struct {
    bool secure;
} NEXUS_Graphics2DOpenSettings;

// Surface Client structures
typedef struct {
    unsigned width;
    unsigned height;
    struct {
        struct {
            unsigned width;
            unsigned height;
        } framebuffer;
    } display;
} NEXUS_SurfaceClientStatus;

typedef struct {
    NEXUS_CallbackDesc displayStatusChanged;
    NEXUS_CallbackDesc displayed;  // Display completion callback
} NEXUS_SurfaceClientSettings;

// Blend equation for composition
typedef  struct {
    NEXUS_BlendFactor colorSourceFactor;
    NEXUS_BlendFactor alphaSourceFactor;
    bool enableColorBlend;
    NEXUS_BlendFactor colorDestFactor;
    NEXUS_BlendFactor alphaDestFactor;
    bool enableAlphaBlend;
    NEXUS_BlendFactor colorOp;
} NEXUS_BlendEquation;

// Color matrix for composition
typedef struct {
    int32_t shift;
    int32_t coeffMatrix[20];  // Array size must accommodate coeffMatrix[18]
    int32_t coeffOffset[3];
} NEXUS_Graphics2DColorMatrix;

// Surface composition structure
typedef struct {
    NEXUS_Rect position;
    NEXUS_Rect clipRect;
    bool visible;
    unsigned zorder;
    NEXUS_BlendEquation colorBlend;
    NEXUS_BlendEquation alphaBlend;
    NEXUS_Graphics2DColorMatrix colorMatrix;
    bool colorMatrixEnabled;
    bool virtualDisplay;
} NEXUS_SurfaceComposition;

// Client configuration
typedef struct {
    NEXUS_HeapHandle heap[10];
} NEXUS_ClientConfiguration;

// Platform functions
void NEXUS_Platform_GetClientConfiguration(NEXUS_ClientConfiguration *pConfig);
NEXUS_HeapHandle NEXUS_Platform_GetFramebufferHeap(int type);

// Surface functions
void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *pSettings);
NEXUS_SurfaceHandle NEXUS_Surface_Create(const NEXUS_SurfaceCreateSettings *pSettings);
void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle surface);
NEXUS_Error NEXUS_Surface_GetStatus(NEXUS_SurfaceHandle surface, NEXUS_SurfaceStatus *pStatus);
NEXUS_Error NEXUS_Surface_GetMemory(NEXUS_SurfaceHandle surface, NEXUS_SurfaceMemory *pMemory);
void NEXUS_Surface_Flush(NEXUS_SurfaceHandle surface);

// Graphics2D functions
void NEXUS_Graphics2D_GetDefaultOpenSettings(NEXUS_Graphics2DOpenSettings *pSettings);
NEXUS_Graphics2DHandle NEXUS_Graphics2D_Open(unsigned index, NEXUS_Graphics2DOpenSettings *pSettings);
void NEXUS_Graphics2D_Close(NEXUS_Graphics2DHandle handle);
void NEXUS_Graphics2D_GetSettings(NEXUS_Graphics2DHandle handle, NEXUS_Graphics2DSettings *pSettings);
NEXUS_Error NEXUS_Graphics2D_SetSettings(NEXUS_Graphics2DHandle handle, const NEXUS_Graphics2DSettings *pSettings);
void NEXUS_Graphics2D_GetDefaultBlitSettings(NEXUS_Graphics2DBlitSettings *pSettings);
NEXUS_Error NEXUS_Graphics2D_Blit(NEXUS_Graphics2DHandle handle, const NEXUS_Graphics2DBlitSettings *pSettings);
NEXUS_Error NEXUS_Graphics2D_Checkpoint(NEXUS_Graphics2DHandle handle, void* pSettings);

// BKNI event functions
NEXUS_Error BKNI_CreateEvent(BKNI_EventHandle *event);
void BKNI_DestroyEvent(BKNI_EventHandle event);
NEXUS_Error BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMs);
void BKNI_SetEvent(BKNI_EventHandle event);

// Surface Client functions
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire(unsigned surfaceClientId);
void NEXUS_SurfaceClient_Release(NEXUS_SurfaceClientHandle handle);
NEXUS_Error NEXUS_SurfaceClient_GetStatus(NEXUS_SurfaceClientHandle handle, NEXUS_SurfaceClientStatus *pStatus);
void NEXUS_SurfaceClient_GetSettings(NEXUS_SurfaceClientHandle handle, NEXUS_SurfaceClientSettings *pSettings);
NEXUS_Error NEXUS_SurfaceClient_SetSettings(NEXUS_SurfaceClientHandle handle, const NEXUS_SurfaceClientSettings *pSettings);

// NxClient composition functions
NEXUS_Error NxClient_GetSurfaceClientComposition(unsigned surfaceClientId, NEXUS_SurfaceComposition *pComposition);
NEXUS_Error NxClient_SetSurfaceClientComposition(unsigned surfaceClientId, const NEXUS_SurfaceComposition *pComposition);

// NXPL (Nexus Platform Layer) types and functions
typedef void* NXPL_PlatformHandle;
typedef void* NXPL_NativeWindowHandle;

typedef enum {
    BEGL_BufferFormat_eA8B8G8R8,
    BEGL_BufferFormat_eR8G8B8A8
} BEGL_BufferFormat;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool stretch;
    unsigned clientID;
    int zOrder;
} NXPL_NativeWindowInfo;

typedef struct {
    int width;
    int height;
    BEGL_BufferFormat format;
} BEGL_PixmapInfo;

typedef struct {
    int width;
    int height;
    BEGL_BufferFormat format;
    bool secure;
} BEGL_PixmapInfoEXT;

NXPL_PlatformHandle NXPL_CreatePlatform(void);
void NXPL_DestroyPlatform(NXPL_PlatformHandle platform);
NXPL_NativeWindowHandle NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info);
void NXPL_DestroyNativeWindow(NXPL_NativeWindowHandle window);
int NXPL_GetNativeWindowInfo(NXPL_NativeWindowHandle window, NXPL_NativeWindowInfo *info);
int NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle* platform, int displayNum);
void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle platform);
void NXPL_GetDefaultPixmapInfoEXT(BEGL_PixmapInfoEXT *info);
bool NXPL_CreateCompatiblePixmapEXT(NXPL_PlatformHandle platform, void **pixmap, void **surface, const BEGL_PixmapInfoEXT *info);
bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle platform, void **pixmap, void **surface, const BEGL_PixmapInfo *info);
void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle platform, void *pixmap);

#ifdef __cplusplus
}
#endif

#endif // NEXUS_PLATFORM_H
