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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "request.h"
#include "socket.h"
#include "log.h"

#ifdef __MINGW32__
#include "win32.h"
#endif

#define PROG_NAME "lightjbl"
#define PROG_VER "0.2"
#define SAFE_STR(x) (x ? x : "(null)")

void print_help()
{
    fprintf(stderr, "Lightweight JetBrains License Server\n" \
                    "v" PROG_VER " by @privatelo\n" \
                    "For testing purpose only. Please support genuine!\n\n" \
                    "Usage: " PROG_NAME " [option]\n" \
                    "options:\n" \
                    "\t-h           show help\n" \
                    "\t-v           verbose mode\n" \
                    "\t-p <port>    port to listen (default: 8080)\n" \
                    "\t-l <address> address to listen (default: 0.0.0.0)\n" \
                    "\t-u <name>    name of licensee (default: privatelo)\n" \
                    "\t-t <day>     prolongation period (default: 7)\n" \
                    "\t-i <file>    static html file for homepage\n" \
            );
}

int main(int argc, const char *argv[])
{
    char opt;
    int prolong_opt = 0;
    char *licensee_opt = NULL;
    char *port_opt = NULL;
    char *addr_opt = NULL;
    char *html_opt = NULL;
    int verbose_opt = 0;
    int exit_prog = 0;
    int show_help = 0;
    int ret = 0;
    while ((opt = getopt(argc, (char * const *) argv, "hvp:l:u:t:i:")) != -1) {
        switch (opt) {
            case 'h':
                exit_prog = 1;
                show_help = 1;
                break;
            case 'v':
                verbose_opt = 1;
                LOG(INFO, "enabled verbose mode");
                break;
            case 'p':
                port_opt = optarg;
                break;
            case 'l':
                addr_opt = optarg;
                break;
            case 'i':
                html_opt = optarg;
                break;
            case 'u':
                licensee_opt = optarg;
                LOG(INFO, "set licensee to %s", SAFE_STR(licensee_opt));
                break;
            case 't':
                prolong_opt = atoi(optarg);
                if (prolong_opt < 1) {
                    fprintf(stderr, PROG_NAME ": prolongation period must be a positive number\n");
                    exit_prog = 1;
                } else {
                    LOG(INFO, "set prolongation period to %d days", prolong_opt);
                }
                break;
            default:
                exit_prog = 1;
                break;
        }
    }
    if (exit_prog) {
        if (show_help) {
            print_help();
            return 0;
        }
        return 1;
    }

    struct socket_server server = {0};
    settings setting = {0};
    server.addr = addr_opt ? addr_opt : "0.0.0.0";
    server.port = port_opt ? port_opt : "8080";
    server.handle_request = request_handler;
    setting.licensee = licensee_opt;
    setting.prolong = ((uint64_t) prolong_opt) * 24 * 3600 * 1000;
    setting.homepage = sdsempty();
    setting.verbose = verbose_opt;
    if (html_opt) {
        char *source = NULL;
        FILE *fp = fopen(html_opt, "rb");
        if (fp != NULL) {
            if (fseek(fp, 0L, SEEK_END) == 0) {
                long bufsize = ftell(fp);
                if (bufsize == -1) {
                    goto FAIL;
                }
                if (fseek(fp, 0L, SEEK_SET) != 0) {
                    goto FAIL;
                }
                source = malloc(sizeof(char) * (bufsize + 1));
                if (source == NULL) {
                    goto FAIL;
                }
                size_t newLen = fread(source, sizeof(char), bufsize, fp);
                if (ferror(fp) == 0) {
                    setting.homepage = sdscatlen(setting.homepage, source, newLen);
                    LOG(INFO, "homepage loaded from %s", html_opt);
                }
                free(source);
                goto SUCCESS;
            }
FAIL:
            LOG(ERROR, "failed to read homepage file \"%s\"", html_opt);
SUCCESS:
            fclose(fp);
        } else {
            LOGERR("cannot access file \"%s\"", html_opt);
        }
    }
    server.userinfo = (void *) &setting;
    server.verbose = verbose_opt;
    ret = socket_server_loop(&server);
    sdsfree(setting.homepage);
    LOG(INFO, "stopped gracefully");
    return ret;
}
