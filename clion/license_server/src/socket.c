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

#include "socket.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef __MINGW32__
#include "win32.h"
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#define REQUEST_BUFFER_SIZE 2048

struct client {
    int fd;
    ev_io ev_accept;
    ev_io ev_read;
    ev_io ev_write;
    char *request_data;
    size_t data_len;
    void (*handle_request)(char *request, size_t len, void *user, int fd);
    void *user;
    int verbose;
};

#ifndef __MINGW32__
static inline int setnonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) return -1;
    return 0;
}
#endif

static void write_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
    if (!(revents & EV_WRITE)) {
        ev_io_stop(EV_A_ w);
        return;
    }
    struct client *client =
        (struct client *)
        (((char *) w) - offsetof(struct client, ev_write));
    if (client == NULL) {
        ev_io_stop(EV_A_ w);
        return;
    }
    if (client->request_data != NULL) {
        client->handle_request(client->request_data, client->data_len,
                               client->user, client->fd);
        free(client->request_data);
    }
    ev_io_stop(EV_A_ w);
    close(client->fd);
    free(client);
}

static void read_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
    if (!(revents & EV_READ)) {
        ev_io_stop(EV_A_ w);
        return;
    }
    struct client *client =
        (struct client *)
        (((char *) w) - offsetof(struct client, ev_read));
    if (client == NULL) {
        ev_io_stop(EV_A_ w);
        return;
    }
    char rbuff[REQUEST_BUFFER_SIZE + 1];
    size_t sum = 0, len = 0;
    client->request_data = NULL;
    do {
        len = read(client->fd, rbuff, REQUEST_BUFFER_SIZE);
        sum += len;
        if (len < REQUEST_BUFFER_SIZE)
            rbuff[len] = '\0';
        if (client->request_data == NULL) {
            client->request_data = malloc(len + 1);
            memcpy(client->request_data, rbuff, len);
        } else {
            client->request_data = realloc(client->request_data, sum + 1);
            memcpy(client->request_data + sum - len, rbuff, len);
        }
    } while (len == REQUEST_BUFFER_SIZE);
    client->data_len = sum;
    ev_io_stop(EV_A_ w);
    ev_io_init(&client->ev_write, write_cb, client->fd, EV_WRITE);
    ev_io_start(loop, &client->ev_write);
}

static void accept_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
    struct client *main_client =
        (struct client *)
        (((char *) w) - offsetof(struct client, ev_accept));
    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(w->fd, (struct sockaddr *) &client_addr, &client_len);
    if (client_fd == -1) {
        return;
    }
    if (setnonblock(client_fd) < 0) {
#ifdef DEBUG
        LOGERR("failed to set client socket to nonblock");
#endif
    }
    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];
    int ret = getnameinfo((struct sockaddr *) &client_addr, client_len,
                          hoststr, sizeof(hoststr),
                          portstr, sizeof(portstr),
                          NI_NUMERICHOST | NI_NUMERICSERV);
    
    if (ret == 0 && main_client->verbose) {
        LOG(INFO, "connection from %s:%s", hoststr, portstr);
    }
    struct client *client = malloc(sizeof(struct client));
    client->handle_request = main_client->handle_request;
    client->user = main_client->user;
    client->verbose = main_client->verbose;
    client->fd = client_fd;
    ev_io_init(&client->ev_read, read_cb, client->fd, EV_READ);
    ev_io_start(loop, &client->ev_read);
}

static void signal_cb(EV_P_ ev_signal *w, int revents)
{
    if (revents & EV_SIGNAL) {
        switch (w->signum) {
            case SIGINT:
            case SIGTERM:
              ev_unloop(EV_A_ EVUNLOOP_ALL);
        }
    }
}

int socket_server_loop(struct socket_server *server)
{
    int ret = -1;
    if (server == NULL) {
        LOG(ERROR, "null server context");
        return ret;
    }
#ifdef __MINGW32__
    if (winsock_init() < 0) {
        return ret;
    };
#endif
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    LOG(INFO, "starting license server on %s:%s", server->addr, server->port);
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int s = getaddrinfo(server->addr, server->port, &hints, &result);
    if (s != 0 || result == NULL) {
        LOGERR("listen failed(getaddrinfo)");
        goto FAIL;
    }
    int listen_fd = -1;
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (listen_fd < 0) {
            LOGERR("listen failed(socket)");
            continue;
        }
        int opt = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        } else {
            LOGERR("bind failed");
        }
        close(listen_fd);
        listen_fd = -1;
    }
    freeaddrinfo(result);
    if (listen_fd < 0) {
        goto FAIL;
    }
    if (listen(listen_fd, SOMAXCONN) < 0) {
        LOGERR("listen failed(listen)");
        close(listen_fd);
        goto FAIL;
    }
    if (setnonblock(listen_fd) < 0 && server->verbose) {
        LOGERR("nonblock failed(listen)");
    }
    LOG(INFO, "server started");
    struct ev_signal sigint_watcher;
    struct ev_signal sigterm_watcher;
    ev_signal_init(&sigint_watcher, signal_cb, SIGINT);
    ev_signal_init(&sigterm_watcher, signal_cb, SIGTERM);
    ev_signal_start(EV_DEFAULT, &sigint_watcher);
    ev_signal_start(EV_DEFAULT, &sigterm_watcher);
    struct client *main_client = malloc(sizeof(struct client));
    main_client->handle_request = server->handle_request;
    main_client->user = server->userinfo;
    main_client->verbose = server->verbose;
    server->loop = ev_default_loop(0);
    ev_io_init(&main_client->ev_accept, accept_cb, listen_fd, EV_READ);
    ev_io_start(server->loop, &main_client->ev_accept);
    server->ev_accept = &main_client->ev_accept;
    ev_loop(server->loop, 0);
    ev_signal_stop(EV_DEFAULT, &sigint_watcher);
    ev_signal_stop(EV_DEFAULT, &sigterm_watcher);
    ret = 0;
FAIL:
#ifdef __MINGW32__
    winsock_cleanup();
#endif
    return ret;
}
