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

#ifndef JBLS_C_SOCKET_H
#define JBLS_C_SOCKET_H 1

#include <ev.h>

struct socket_server {
    char *addr;
    char *port;
    void *userinfo;
    int verbose;
    void (*handle_request)(char *request, size_t len, void *user, int fd);
    struct ev_loop *loop;
    struct ev_io *ev_accept;
};

int socket_server_loop(struct socket_server *server);

#endif
