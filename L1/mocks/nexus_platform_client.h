/*
 * Mock NEXUS Platform Client Header for L1 Testing
 * Minimal definitions for nexus_platform_client.h
 */

#ifndef NEXUS_PLATFORM_CLIENT_H__
#define NEXUS_PLATFORM_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

// Platform client initialization
typedef struct {
    unsigned unused;
} NEXUS_PlatformClientSettings;

// Mock functions (can be empty for testing)
static inline void NEXUS_Platform_GetDefaultClientSettings(NEXUS_PlatformClientSettings *pSettings) {
    if (pSettings) {
        pSettings->unused = 0;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_PLATFORM_CLIENT_H__ */
