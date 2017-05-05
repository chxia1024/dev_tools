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

#ifndef JBLS_C_WIN32_H
#define JBLS_C_WIN32_H

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef errno
#undef errno
#endif

#ifndef AI_ALL
#define AI_ALL 0x00000100
#endif

#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0x00000400
#endif

#ifndef AI_V4MAPPED
#define AI_V4MAPPED 0x00000800
#endif

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27 // Treat wildcard bind as AF_INET6-only.
#endif

#define read(...) recv(__VA_ARGS__, 0)
#define write(...) send(__VA_ARGS__, 0)
#define close(fd) closesocket(fd)
#define setsockopt(a, b, c, d, e) setsockopt(a, b, c, (char *)(d), e)

int winsock_init(void);
void winsock_cleanup(void);
int setnonblock(int fd);

#endif
