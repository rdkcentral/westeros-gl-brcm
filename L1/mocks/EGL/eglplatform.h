/*
 * Mock EGL Platform Header for L1 Testing
 * Minimal definitions for platform-specific types
 */

#ifndef __eglplatform_h_
#define __eglplatform_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Platform-specific types */
#ifndef EGLAPI
#define EGLAPI extern
#endif

#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif

#ifndef EGLAPIENTRYP
#define EGLAPIENTRYP EGLAPIENTRY *
#endif

/* EGL native types - use void pointers for mock */
typedef void *EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;

#ifdef __cplusplus
}
#endif

#endif /* __eglplatform_h_ */
