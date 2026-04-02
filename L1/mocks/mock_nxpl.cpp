/**
 * Mock implementation for NXPL (Nexus Platform Layer) APIs
 * Used for L1 testing without hardware dependencies
 */

#include <cstdlib>
#include <cstring>
#include <set>
#include "nexus_platform.h"

// Mock state tracking
namespace MockNXPL {
    bool g_platformSuccess = true;
    bool g_windowCreationSuccess = true;
    int g_registerCount = 0;
    int g_unregisterCount = 0;
    std::set<void*> g_allocatedWindows;
    std::set<void*> g_allocatedPixmaps;
    
    void reset() {
        g_platformSuccess = true;
        g_windowCreationSuccess = true;
        g_registerCount = 0;
        g_unregisterCount = 0;
        g_allocatedWindows.clear();
        g_allocatedPixmaps.clear();
    }
    
    void setPlatformSuccess(bool success) {
        g_platformSuccess = success;
    }
    
    void setWindowCreationSuccess(bool success) {
        g_windowCreationSuccess = success;
    }
    
    int getRegisterCount() {
        return g_registerCount;
    }
    
    int getUnregisterCount() {
        return g_unregisterCount;
    }
}

// Mock NXPL APIs
extern "C" {

NXPL_PlatformHandle NXPL_CreatePlatform(void) {
    if (!MockNXPL::g_platformSuccess) {
        return NULL;
    }
    
    return (NXPL_PlatformHandle)0x11223344;
}

void NXPL_DestroyPlatform(NXPL_PlatformHandle platform) {
    (void)platform;
}

NXPL_NativeWindowHandle NXPL_CreateNativeWindow(const NXPL_NativeWindowInfo *info) {
    if (!info || !MockNXPL::g_windowCreationSuccess) {
        return NULL;
    }
    
    void* window = malloc(sizeof(NXPL_NativeWindowInfo));
    if (window) {
        memcpy(window, info, sizeof(NXPL_NativeWindowInfo));
        MockNXPL::g_allocatedWindows.insert(window);
    }
    
    return (NXPL_NativeWindowHandle)window;
}

void NXPL_DestroyNativeWindow(NXPL_NativeWindowHandle window) {
    if (window) {
        // Only free if this window was actually allocated and not already freed
        if (MockNXPL::g_allocatedWindows.find(window) != MockNXPL::g_allocatedWindows.end()) {
            MockNXPL::g_allocatedWindows.erase(window);
            free(window);
        }
        // If window not in set, it's either already freed or invalid - ignore safely
    }
}

int NXPL_GetNativeWindowInfo(NXPL_NativeWindowHandle window, NXPL_NativeWindowInfo *info) {
    if (!window || !info) {
        return -1;
    }
    
    memcpy(info, window, sizeof(NXPL_NativeWindowInfo));
    return 0;
}

int NXPL_RegisterNexusDisplayPlatform(NXPL_PlatformHandle* platform, int displayNum) {
    (void)displayNum;
    
    if (!platform) {
        return -1;
    }
    
    *platform = (NXPL_PlatformHandle)0x11223344;
    MockNXPL::g_registerCount++;
    return 0;
}

void NXPL_UnregisterNexusDisplayPlatform(NXPL_PlatformHandle platform) {
    (void)platform;
    
    MockNXPL::g_unregisterCount++;
}

void NXPL_GetDefaultPixmapInfoEXT(BEGL_PixmapInfoEXT *info) {
    if (info) {
        info->width = 1920;
        info->height = 1080;
        info->format = BEGL_BufferFormat_eA8B8G8R8;
        info->secure = false;
    }
}

bool NXPL_CreateCompatiblePixmapEXT(NXPL_PlatformHandle platform, void **pixmap, void **surface, const BEGL_PixmapInfoEXT *info) {
    (void)platform;
    
    if (!pixmap || !surface || !info) {
        return false;
    }
    
    // Create mock pixmap and surface
    *pixmap = malloc(128);
    *surface = malloc(256);
    
    if (*pixmap) {
        MockNXPL::g_allocatedPixmaps.insert(*pixmap);
    }
    
    return (*pixmap != nullptr && *surface != nullptr);
}

bool NXPL_CreateCompatiblePixmap(NXPL_PlatformHandle platform, void **pixmap, void **surface, const BEGL_PixmapInfo *info) {
    (void)platform;
    
    if (!pixmap || !surface || !info) {
        return false;
    }
    
    // Create mock pixmap and surface
    *pixmap = malloc(128);
    *surface = malloc(256);
    
    if (*pixmap) {
        MockNXPL::g_allocatedPixmaps.insert(*pixmap);
    }
    
    return (*pixmap != nullptr && *surface != nullptr);
}

void NXPL_DestroyCompatiblePixmap(NXPL_PlatformHandle platform, void *pixmap) {
    (void)platform;
    
    if (pixmap) {
        // Only free if this pixmap was actually allocated and not already freed
        if (MockNXPL::g_allocatedPixmaps.find(pixmap) != MockNXPL::g_allocatedPixmaps.end()) {
            MockNXPL::g_allocatedPixmaps.erase(pixmap);
            free(pixmap);
        }
        // If pixmap not in set, it's either already freed or invalid - ignore safely
    }
}

} // extern "C"
