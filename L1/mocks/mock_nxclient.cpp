/**
 * Mock implementation for NxClient APIs
 * Used for L1 testing without hardware dependencies
 */

#include <cstdlib>
#include <cstring>
#include <string>
#include "nxclient.h"
#include "nexus_platform.h"

// Mock state tracking
namespace MockNxClient {
    bool g_joinSuccess = true;
    bool g_allocSuccess = true;
    int g_joinCount = 0;
    int g_uninitCount = 0;
    unsigned g_displayWidth = 1920;
    unsigned g_displayHeight = 1080;
    
    void reset() {
        g_joinSuccess = true;
        g_allocSuccess = true;
        g_joinCount = 0;
        g_uninitCount = 0;
        g_displayWidth = 1920;
        g_displayHeight = 1080;
    }
    
    void setJoinSuccess(bool success) {
        g_joinSuccess = success;
    }
    
    void setAllocSuccess(bool success) {
        g_allocSuccess = success;
    }
    
    void setDisplaySize(unsigned width, unsigned height) {
        g_displayWidth = width;
        g_displayHeight = height;
    }
    
    int getJoinCount() {
        return g_joinCount;
    }
    
    int getUninitCount() {
        return g_uninitCount;
    }
}

// Mock NxClient APIs
extern "C" {

void NxClient_GetDefaultJoinSettings(NxClient_JoinSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
    }
}

int NxClient_Join(NxClient_JoinSettings *pSettings) {
    (void)pSettings;
    
    if (!MockNxClient::g_joinSuccess) {
        return -1;
    }
    
    MockNxClient::g_joinCount++;
    return 0;
}

void NxClient_Uninit(void) {
    MockNxClient::g_uninitCount++;
}

void NxClient_GetDefaultAllocSettings(NxClient_AllocSettings *pSettings) {
    if (pSettings) {
        memset(pSettings, 0, sizeof(*pSettings));
    }
}

int NxClient_Alloc(NxClient_AllocSettings *pSettings, NxClient_AllocResults *pResults) {
    (void)pSettings;
    
    if (!MockNxClient::g_allocSuccess) {
        return -1;
    }
    
    if (pResults) {
        memset(pResults, 0, sizeof(*pResults));
        pResults->surfaceClient[0].id = 1;
    }
    
    return 0;
}

void NxClient_Free(NxClient_AllocResults *pResults) {
    (void)pResults;
}

int NxClient_GetDisplaySettings(NxClient_DisplaySettings *pSettings) {
    if (!pSettings) {
        return -1;
    }
    
    pSettings->width = MockNxClient::g_displayWidth;
    pSettings->height = MockNxClient::g_displayHeight;
    pSettings->refreshRate = 60;
    pSettings->enabled = 1;
    pSettings->secure = 0;
    
    return 0;
}

int NxClient_SetDisplaySettings(const NxClient_DisplaySettings *pSettings) {
    if (!pSettings) {
        return -1;
    }
    
    MockNxClient::g_displayWidth = pSettings->width;
    MockNxClient::g_displayHeight = pSettings->height;
    
    return 0;
}

int NxClient_RegisterDisplaySettingsChangedCallback(
    NxClient_DisplaySettingsChangedCallback callback,
    void *context,
    int *pId) {
    (void)callback;
    (void)context;
    
    if (pId) {
        *pId = 0x1234;
    }
    
    return 0;
}

void NxClient_UnregisterDisplaySettingsChangedCallback(int id) {
    (void)id;
}

// NEXUS_SurfaceClient_GetStatus - custom implementation with display dimensions
int NEXUS_SurfaceClient_GetStatus(NEXUS_SurfaceClientHandle handle, NEXUS_SurfaceClientStatus* pStatus) {
    (void)handle;
    if (!pStatus) {
        return -1;
    }
    
    pStatus->width = MockNxClient::g_displayWidth;
    pStatus->height = MockNxClient::g_displayHeight;
    pStatus->display.framebuffer.width = MockNxClient::g_displayWidth;
    pStatus->display.framebuffer.height = MockNxClient::g_displayHeight;
    
    return 0;
}

// NEXUS_SurfaceClient_SetSurface - NxClient specific
int NEXUS_SurfaceClient_SetSurface(NEXUS_SurfaceClientHandle handle, NEXUS_SurfaceHandle surface) {
    (void)handle;
    (void)surface;
    return 0;
}

// Note: Other NEXUS_SurfaceClient_* functions (Acquire, Release, GetSettings, SetSettings)
// are implemented in mock_nexus.cpp to avoid duplication

// C wrapper functions for mock control
void mock_nxclient_reset(void) {
    MockNxClient::reset();
}

void mock_nxclient_set_join_failure(bool fail) {
    MockNxClient::setJoinSuccess(!fail);
}

} // extern "C"
