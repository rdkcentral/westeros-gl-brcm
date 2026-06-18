/**
 * Mock implementation for Nexus Graphics2D APIs
 * Used for L1 testing without hardware dependencies
 */

#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <memory>
#include "nexus_platform.h"
#include "nexus_video_decoder.h"

// Mock state tracking
namespace MockNexus {
    bool g_initSuccess = true;
    bool g_failNextAlloc = false;
    int g_openCount = 0;
    int g_closeCount = 0;
    std::map<void*, size_t> g_allocatedSurfaces;
    std::map<void*, bool> g_allocatedSurfaceClients;
    bool g_pushSurfaceSuccess = true;
    bool g_surfaceClientCreateSuccess = true;
    
    void reset() {
        g_initSuccess = true;
        g_failNextAlloc = false;
        g_openCount = 0;
        g_closeCount = 0;
        g_allocatedSurfaces.clear();
        g_allocatedSurfaceClients.clear();
        g_pushSurfaceSuccess = true;
        g_surfaceClientCreateSuccess = true;
    }
    
    void setInitSuccess(bool success) {
        g_initSuccess = success;
    }
    
    void setFailNextAlloc(bool fail) {
        g_failNextAlloc = fail;
    }
    
    int getOpenCount() {
        return g_openCount;
    }
    
    int getCloseCount() {
        return g_closeCount;
    }
}

// Mock Graphics2D APIs
extern "C" {

void NEXUS_Surface_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
        pSettings->width = 1920;
        pSettings->height = 1080;
        pSettings->pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    }
}

void NEXUS_Graphics2D_GetDefaultCreateSettings(NEXUS_SurfaceCreateSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
        pSettings->width = 1920;
        pSettings->height = 1080;
        pSettings->pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    }
}

NEXUS_Graphics2DHandle NEXUS_Graphics2D_Open(unsigned index, NEXUS_Graphics2DOpenSettings* pSettings) {
    (void)index;
    (void)pSettings;
    
    if (!MockNexus::g_initSuccess) {
        return NULL;
    }
    
    MockNexus::g_openCount++;
    return (NEXUS_Graphics2DHandle)0x12345678;
}

void NEXUS_Graphics2D_Close(NEXUS_Graphics2DHandle handle) {
    if (handle) {
        MockNexus::g_closeCount++;
    }
}

NEXUS_SurfaceHandle NEXUS_Surface_Create(const NEXUS_SurfaceCreateSettings *pSettings) {
    if (!pSettings || MockNexus::g_failNextAlloc) {
        return NULL;
    }
    
    size_t size = pSettings->width * pSettings->height * 4; // Assume 4 bytes per pixel
    void* surface = malloc(size + sizeof(NEXUS_SurfaceCreateSettings));
    
    if (surface) {
        // Store settings at the beginning
        memcpy(surface, pSettings, sizeof(NEXUS_SurfaceCreateSettings));
        MockNexus::g_allocatedSurfaces[surface] = size;
    }
    
    return (NEXUS_SurfaceHandle)surface;
}

void NEXUS_Surface_Destroy(NEXUS_SurfaceHandle handle) {
    if (handle) {
        MockNexus::g_allocatedSurfaces.erase(handle);
        free(handle);
    }
}

NEXUS_Error NEXUS_Surface_GetMemory(NEXUS_SurfaceHandle surface, NEXUS_SurfaceMemory *pMemory) {
    if (!surface || !pMemory) {
        return NEXUS_UNKNOWN;
    }
    
    NEXUS_SurfaceCreateSettings* settings = (NEXUS_SurfaceCreateSettings*)surface;
    pMemory->width = settings->width;
    pMemory->height = settings->height;
    
    // Allocate mock buffer if not already allocated
    auto it = MockNexus::g_allocatedSurfaces.find(surface);
    if (it == MockNexus::g_allocatedSurfaces.end()) {
        // Calculate buffer size: width * height * 4 bytes per pixel (ARGB)
        size_t bufferSize = settings->width * settings->height * 4;
        MockNexus::g_allocatedSurfaces[surface] = bufferSize;
    }
    
    // Set pitch (bytes per line) - typically width * bytes_per_pixel
    // For ARGB8888 format: 4 bytes per pixel
    pMemory->pitch = settings->width * 4;
    
    // Allocate or reuse mock surface buffer
    static std::map<void*, std::vector<unsigned char>> surface_buffers;
    size_t bufferSize = pMemory->pitch * pMemory->height;
    
    if (surface_buffers.find(surface) == surface_buffers.end()) {
        surface_buffers[surface].resize(bufferSize, 0);
    }
    pMemory->buffer = surface_buffers[surface].data();
    
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Surface_GetStatus(NEXUS_SurfaceHandle surface, NEXUS_SurfaceStatus *pStatus) {
    if (!surface || !pStatus) {
        return NEXUS_UNKNOWN;
    }
    
    NEXUS_SurfaceCreateSettings* settings = (NEXUS_SurfaceCreateSettings*)surface;
    pStatus->width = settings->width;
    pStatus->height = settings->height;
    pStatus->pixelFormat = settings->pixelFormat;
    
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Graphics2D_Blit(NEXUS_Graphics2DHandle handle, const NEXUS_Graphics2DBlitSettings *pSettings) {
    if (!handle || !pSettings) {
        return NEXUS_UNKNOWN;
    }
    return NEXUS_SUCCESS;
}

void NEXUS_Graphics2D_GetDefaultBlitSettings(NEXUS_Graphics2DBlitSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
    }
}

NEXUS_Error NEXUS_Graphics2D_Checkpoint(NEXUS_Graphics2DHandle handle, void* pSettings) {
    (void)pSettings;
    if (!handle) {
        return NEXUS_UNKNOWN;
    }
    return NEXUS_SUCCESS;
}

void NEXUS_Platform_GetClientConfiguration(NEXUS_ClientConfiguration *pConfig) {
    if (pConfig) {
        memset(pConfig, 0, sizeof(*pConfig));
        // Initialize heap array to null
        for (int i = 0; i < 10; i++) {
            pConfig->heap[i] = nullptr;
        }
    }
}

NEXUS_HeapHandle NEXUS_Platform_GetFramebufferHeap(int type) {
    (void)type;
    // Return a mock heap handle
    return (NEXUS_HeapHandle)0x11223344;
}

NEXUS_Error BKNI_CreateEvent(BKNI_EventHandle *event) {
    if (!event) {
        return NEXUS_UNKNOWN;
    }
    
    *event = (BKNI_EventHandle)malloc(64);
    return NEXUS_SUCCESS;
}

void BKNI_DestroyEvent(BKNI_EventHandle event) {
    if (event) {
        free(event);
    }
}

NEXUS_Error BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMs) {
    (void)event;
    (void)timeoutMs;
    // Immediate return for testing
    return NEXUS_SUCCESS;
}

void BKNI_SetEvent(BKNI_EventHandle event) {
    (void)event;
}

void NEXUS_Graphics2D_GetDefaultOpenSettings(NEXUS_Graphics2DOpenSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
    }
}

void NEXUS_Graphics2D_GetSettings(NEXUS_Graphics2DHandle handle, NEXUS_Graphics2DSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
    }
    (void)handle;
}

NEXUS_Error NEXUS_Graphics2D_SetSettings(NEXUS_Graphics2DHandle handle, const NEXUS_Graphics2DSettings *pSettings) {
    (void)handle;
    (void)pSettings;
    
    if (!handle) {
        return NEXUS_UNKNOWN;
    }
    
    return NEXUS_SUCCESS;
}

void NEXUS_GetVideoDecoderCapabilities(NEXUS_VideoDecoderCapabilities *pCapabilities) {
    if (pCapabilities) {
        memset(pCapabilities, 0, sizeof(*pCapabilities));
        pCapabilities->memory[0].secure = NEXUS_SecureVideo_eSecure;
        pCapabilities->memory[1].secure = NEXUS_SecureVideo_eUnsecure;
    }
}

// ========== NEXUS SurfaceClient APIs ==========

// Control functions for test scenarios
void mock_surface_client_set_push_failure(bool fail) {
    MockNexus::g_pushSurfaceSuccess = !fail;
}

void mock_surface_client_set_client_create_failure(bool fail) {
    MockNexus::g_surfaceClientCreateSuccess = !fail;
}

// NEXUS_SurfaceClient_Acquire
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire(unsigned clientId) {
    if (!MockNexus::g_surfaceClientCreateSuccess) {
        return nullptr;
    }
    
    NEXUS_SurfaceClientHandle handle = (NEXUS_SurfaceClientHandle)(0x5000 + clientId);
    MockNexus::g_allocatedSurfaceClients[handle] = true;
    return handle;
}

// NEXUS_SurfaceClient_Release
void NEXUS_SurfaceClient_Release(NEXUS_SurfaceClientHandle client) {
    if (client) {
        MockNexus::g_allocatedSurfaceClients.erase(client);
    }
}

// NEXUS_SurfaceClient_GetSettings
void NEXUS_SurfaceClient_GetSettings(NEXUS_SurfaceClientHandle client, 
                                                NEXUS_SurfaceClientSettings *pSettings) {
    (void)client;
    if (pSettings) {
        memset(pSettings, 0, sizeof(NEXUS_SurfaceClientSettings));
        pSettings->displayStatusChanged.callback = nullptr;
        pSettings->displayStatusChanged.context = nullptr;
    }
}

// NEXUS_SurfaceClient_SetSettings
NEXUS_Error NEXUS_SurfaceClient_SetSettings(NEXUS_SurfaceClientHandle client, 
                                                       const NEXUS_SurfaceClientSettings *pSettings) {
    (void)client;
    (void)pSettings;
    return NEXUS_SUCCESS;
}

// NEXUS_SurfaceClient_PushSurface
NEXUS_Error NEXUS_SurfaceClient_PushSurface(NEXUS_SurfaceClientHandle client,
                                                       NEXUS_SurfaceHandle surface,
                                                       const NEXUS_Rect *rect,
                                                       bool synchronous) {
    (void)client;
    (void)surface;
    (void)rect;
    (void)synchronous;
    
    if (!MockNexus::g_pushSurfaceSuccess) {
        return NEXUS_UNKNOWN;
    }
    
    return NEXUS_SUCCESS;
}

// NEXUS_SurfaceClient_RecycleSurface
NEXUS_Error NEXUS_SurfaceClient_RecycleSurface(NEXUS_SurfaceClientHandle client,
                                                          NEXUS_SurfaceHandle *pRecycledSurfaces,
                                                          unsigned maxSurfaces,
                                                          unsigned *pNumReturned) {
    (void)client;
    (void)pRecycledSurfaces;
    (void)maxSurfaces;
    
    if (pNumReturned) {
        *pNumReturned = 0;
    }
    
    return NEXUS_SUCCESS;
}

// NEXUS_SurfaceClient_Clear
NEXUS_Error NEXUS_SurfaceClient_Clear(NEXUS_SurfaceClientHandle client) {
    (void)client;
    return NEXUS_SUCCESS;
}

// NEXUS_SurfaceClient_UpdateSurface
NEXUS_Error NEXUS_SurfaceClient_UpdateSurface(NEXUS_SurfaceClientHandle client,
                                                         NEXUS_SurfaceHandle surface) {
    (void)client;
    (void)surface;
    return NEXUS_SUCCESS;
}

// NEXUS_Surface_GetCreateSettings
void NEXUS_Surface_GetCreateSettings(NEXUS_SurfaceHandle surface, NEXUS_SurfaceCreateSettings *pSettings) {
    if (surface && pSettings) {
        NEXUS_SurfaceCreateSettings* settings = (NEXUS_SurfaceCreateSettings*)surface;
        memcpy(pSettings, settings, sizeof(NEXUS_SurfaceCreateSettings));
    }
}

// NEXUS_Surface_Flush
void NEXUS_Surface_Flush(NEXUS_SurfaceHandle surface) {
    (void)surface;
}

// NxClient surface composition functions
NEXUS_Error NxClient_GetSurfaceClientComposition(unsigned surfaceClientId, NEXUS_SurfaceComposition *pComposition) {
    (void)surfaceClientId;
    if (pComposition) {
        // Initialize with default values
        memset(pComposition, 0, sizeof(NEXUS_SurfaceComposition));
        pComposition->visible = true;
        pComposition->position.x = 0;
        pComposition->position.y = 0;
        pComposition->position.width = 1920;
        pComposition->position.height = 1080;
        pComposition->zorder = 0;
        
        // Initialize colorBlend equation
        pComposition->colorBlend.colorSourceFactor = NEXUS_BlendFactor_eSourceAlpha;
        pComposition->colorBlend.alphaSourceFactor = NEXUS_BlendFactor_eOne;
        pComposition->colorBlend.enableColorBlend = true;
        pComposition->colorBlend.colorDestFactor = NEXUS_BlendFactor_eInverseSourceAlpha;
        pComposition->colorBlend.alphaDestFactor = NEXUS_BlendFactor_eZero;
        pComposition->colorBlend.enableAlphaBlend = true;
        pComposition->colorBlend.colorOp = NEXUS_BlendFactor_eOne;
        
        // Initialize alphaBlend equation
        pComposition->alphaBlend.colorSourceFactor = NEXUS_BlendFactor_eOne;
        pComposition->alphaBlend.alphaSourceFactor = NEXUS_BlendFactor_eOne;
        pComposition->alphaBlend.enableColorBlend = true;
        pComposition->alphaBlend.colorDestFactor = NEXUS_BlendFactor_eZero;
        pComposition->alphaBlend.alphaDestFactor = NEXUS_BlendFactor_eZero;
        pComposition->alphaBlend.enableAlphaBlend = true;
        pComposition->alphaBlend.colorOp = NEXUS_BlendFactor_eOne;
    }
    return NEXUS_SUCCESS;
}

NEXUS_Error NxClient_SetSurfaceClientComposition(unsigned surfaceClientId, const NEXUS_SurfaceComposition *pComposition) {
    (void)surfaceClientId;
    (void)pComposition;
    // Accept all settings in mock
    return NEXUS_SUCCESS;
}

// C wrapper for mock control functions
void MockNexus_reset(void) {
    MockNexus::reset();
}

} // extern "C"
