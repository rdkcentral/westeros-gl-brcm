/**
 * Mock NXPL header for L1 testing
 */
#ifndef NXPL_H
#define NXPL_H

#ifdef __cplusplus
extern "C" {
#endif

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

#endif // NXPL_H
