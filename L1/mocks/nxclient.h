/**
 * Mock NxClient header for L1 testing
 */
#ifndef NXCLIENT_H
#define NXCLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define NXCLIENT_MAX_NAME 128

typedef void* NEXUS_SurfaceClientHandle;

typedef struct {
    char name[NXCLIENT_MAX_NAME];
} NxClient_JoinSettings;

typedef struct {
    unsigned surfaceClient;
} NxClient_AllocSettings;

typedef struct {
    struct {
        unsigned id;
    } surfaceClient[2];
} NxClient_AllocResults;

typedef struct {
    unsigned width;
    unsigned height;
    unsigned refreshRate;
    int enabled;
    int secure;
} NxClient_DisplaySettings;

typedef void (*NxClient_DisplaySettingsChangedCallback)(void *context, int param);

void NxClient_GetDefaultJoinSettings(NxClient_JoinSettings *pSettings);
int NxClient_Join(NxClient_JoinSettings *pSettings);
void NxClient_Uninit(void);
void NxClient_GetDefaultAllocSettings(NxClient_AllocSettings *pSettings);
int NxClient_Alloc(NxClient_AllocSettings *pSettings, NxClient_AllocResults *pResults);
void NxClient_Free(NxClient_AllocResults *pResults);
int NxClient_GetDisplaySettings(NxClient_DisplaySettings *pSettings);
int NxClient_SetDisplaySettings(const NxClient_DisplaySettings *pSettings);
int NxClient_RegisterDisplaySettingsChangedCallback(
    NxClient_DisplaySettingsChangedCallback callback,
    void *context,
    int *pId);
void NxClient_UnregisterDisplaySettingsChangedCallback(int id);

#ifdef __cplusplus
}
#endif

#endif // NXCLIENT_H
