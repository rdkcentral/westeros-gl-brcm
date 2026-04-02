/*
 * Mock NEXUS Surface Client Header for L1 Testing
 * Surface client API definitions for compositor
 */

#ifndef NEXUS_SURFACE_CLIENT_H__
#define NEXUS_SURFACE_CLIENT_H__

#include "nexus_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// Surface Client Handle (defined in nexus_platform.h)
// typedef void* NEXUS_SurfaceClientHandle;

// Surface Client Status (defined in nexus_platform.h)

// Function declarations (already in nexus_platform.h or mock_nexus.cpp)
// Included here for completeness
extern NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire(unsigned clientId);
extern void NEXUS_SurfaceClient_Release(NEXUS_SurfaceClientHandle client);
extern void NEXUS_SurfaceClient_GetSettings(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientSettings *pSettings);
extern NEXUS_Error NEXUS_SurfaceClient_SetSettings(NEXUS_SurfaceClientHandle client, const NEXUS_SurfaceClientSettings *pSettings);
extern NEXUS_Error NEXUS_SurfaceClient_PushSurface(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface, const NEXUS_Rect *rect, bool synchronous);
extern NEXUS_Error NEXUS_SurfaceClient_RecycleSurface(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle *pRecycledSurfaces, unsigned maxSurfaces, unsigned *pNumReturned);
extern NEXUS_Error NEXUS_SurfaceClient_Clear(NEXUS_SurfaceClientHandle client);
extern NEXUS_Error NEXUS_SurfaceClient_UpdateSurface(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_SURFACE_CLIENT_H__ */
