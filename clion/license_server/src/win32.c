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

#include "win32.h"
#include "log.h"

#ifdef setsockopt
#undef setsockopt
#endif

int winsock_init(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    wVersionRequested = MAKEWORD(2, 2);
    ret               = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
        LOG(ERROR, "could not initialize winsock");
        return -1;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        LOG(ERROR, "could not find a usable version of winsock");
        return -1;
    }
    return 0;
}

void winsock_cleanup(void)
{
    WSACleanup();
}

int setnonblock(int fd)
{
    u_long iMode = 1;
    long int iResult;
    iResult = ioctlsocket(fd, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        LOG(ERROR, "ioctlsocket failed with error: %ld\n", iResult);
    }
    return iResult;
}
