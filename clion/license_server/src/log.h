/*
 * lightjbl
 * Copyright (C) 2017 privatelo
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JBLS_C_LOG_H
#define JBLS_C_LOG_H

#include <stdio.h>
#include <time.h>
#ifndef __MINGW32__
#include <errno.h>
#endif

#ifdef NOLOG
#define LOG(level, fmt, ...) do {} while (0)
#else
#define LOG(level, fmt, ...) \
  do { \
    char timestr[30]; \
    time_t now = time(0); \
    strftime(timestr, 30, "%Y-%m-%d %H:%M:%S", localtime(&now)); \
    fprintf(stderr, "%s [" #level "] " fmt "\n", timestr, \
            ## __VA_ARGS__); \
  } while (0)
#endif

#ifdef __MINGW32__
#define LOGERR(fmt, ...) \
  do { \
    LPVOID *msg = NULL; \
    FormatMessage( \
        FORMAT_MESSAGE_ALLOCATE_BUFFER | \
        FORMAT_MESSAGE_FROM_SYSTEM | \
        FORMAT_MESSAGE_IGNORE_INSERTS, \
        NULL, WSAGetLastError(), \
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
        (LPTSTR) &msg, 0, NULL); \
    if (msg != NULL) { \
        LOG(ERROR, fmt ": %s", ## __VA_ARGS__, (char *) msg); \
        LocalFree(msg); \
    } \
  } while (0)
#else
#define LOGERR(fmt, ...) \
  LOG(ERROR, fmt ": %s", ## __VA_ARGS__, strerror(errno))
#endif

#endif
