/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _WESTEROS_GL_LOG_H
#define _WESTEROS_GL_LOG_H

#include <stdio.h>
#include <stdarg.h>

/*
 * gLogLevel  - controls verbosity; set from WESTEROS_GL_DEBUG env var at init.
 *   0 = ERROR only  (default)
 *   1 = + WARNING
 *   2 = + INFO
 *   3 = + DEBUG
 *   4 = + TRACE
 */
extern int gLogLevel;

static inline void wstLog( int level, const char *fmt, ... )
{
   if ( level <= gLogLevel )
   {
      va_list argptr;
      va_start( argptr, fmt );
      vfprintf( stderr, fmt, argptr );
      va_end( argptr );
   }
}

#define ERROR(FORMAT, ...)   wstLog(0, FORMAT, ##__VA_ARGS__)
#define WARNING(FORMAT, ...) wstLog(1, FORMAT, ##__VA_ARGS__)
#define INFO(FORMAT, ...)    wstLog(2, FORMAT, ##__VA_ARGS__)
#define DEBUG(FORMAT, ...)   wstLog(3, FORMAT, ##__VA_ARGS__)
#define TRACE(FORMAT, ...)   wstLog(4, FORMAT, ##__VA_ARGS__)

#endif /* _WESTEROS_GL_LOG_H */

