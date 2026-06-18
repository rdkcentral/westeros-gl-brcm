/**
 * Mock implementation for EGL APIs
 * Used for L1 testing without hardware dependencies
 */

#include <cstdlib>
#include <cstring>

// Mock EGL types (simplified)
typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;
typedef void* EGLNativeWindowType;
typedef void* EGLNativePixmapType;
typedef int EGLint;
typedef unsigned int EGLenum;
typedef unsigned int EGLBoolean;

#define EGL_SUCCESS 0x3000
#define EGL_NOT_INITIALIZED 0x3001
#define EGL_BAD_ACCESS 0x3002
#define EGL_BAD_ALLOC 0x3003
#define EGL_FALSE 0
#define EGL_TRUE 1

#define EGL_DEFAULT_DISPLAY ((EGLDisplay)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

// Mock state tracking
namespace MockEGL {
    bool g_initSuccess = true;
    EGLint g_lastError = EGL_SUCCESS;
    int g_displayCount = 0;
    
    void reset() {
        g_initSuccess = true;
        g_lastError = EGL_SUCCESS;
        g_displayCount = 0;
    }
    
    void setInitSuccess(bool success) {
        g_initSuccess = success;
    }
    
    void setLastError(EGLint error) {
        g_lastError = error;
    }
    
    int getDisplayCount() {
        return g_displayCount;
    }
}

// Mock EGL APIs
extern "C" {

EGLDisplay eglGetDisplay(EGLNativeWindowType display_id) {
    (void)display_id;
    
    if (!MockEGL::g_initSuccess) {
        return EGL_NO_DISPLAY;
    }
    
    MockEGL::g_displayCount++;
    return (EGLDisplay)0x22334455;
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) {
    if (!dpy) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return EGL_FALSE;
    }
    
    if (major) *major = 1;
    if (minor) *minor = 4;
    
    MockEGL::g_lastError = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglTerminate(EGLDisplay dpy) {
    if (!dpy) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return EGL_FALSE;
    }
    
    MockEGL::g_lastError = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLint eglGetError(void) {
    return MockEGL::g_lastError;
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                   EGLNativeWindowType win, const EGLint *attrib_list) {
    (void)attrib_list;
    
    if (!dpy || !config || !win) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return EGL_NO_SURFACE;
    }
    
    MockEGL::g_lastError = EGL_SUCCESS;
    return (EGLSurface)0x66778899;
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface) {
    if (!dpy || !surface) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return EGL_FALSE;
    }
    
    MockEGL::g_lastError = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
                           EGLConfig *configs, EGLint config_size, EGLint *num_config) {
    (void)attrib_list;
    
    if (!dpy) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return EGL_FALSE;
    }
    
    if (configs && config_size > 0) {
        configs[0] = (EGLConfig)0xAABBCCDD;
    }
    
    if (num_config) {
        *num_config = 1;
    }
    
    MockEGL::g_lastError = EGL_SUCCESS;
    return EGL_TRUE;
}

const char* eglQueryString(EGLDisplay dpy, EGLint name) {
    (void)name;
    if (!dpy) {
        MockEGL::g_lastError = EGL_BAD_ALLOC;
        return nullptr;
    }
    
    // Return mock extension string that includes Wayland extensions
    MockEGL::g_lastError = EGL_SUCCESS;
    return "EGL_WL_bind_wayland_display EGL_EXT_image_dma_buf_import";
}

void* eglGetProcAddress(const char* procname) {
    (void)procname;
    // Return a dummy function pointer for all requests
    // This allows the production code to retrieve function pointers
    return (void*)0x12345678;
}

} // extern "C"
