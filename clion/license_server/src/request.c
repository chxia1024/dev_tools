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

#include "request.h"
#include "signer.h"
#include "log.h"
#include <unistd.h>
#include <string.h>

#ifdef __MINGW32__
#include "win32.h"
#endif

#define BUFFER_SIZE 256
#define DEF_PROLONG 604800000
#define DEF_USER    "privatelo"
#define DEF_SALT    "1490283405643"
#define DEF_TICK    "1"
#define HTTP_OK     "HTTP/1.1 200 OK\r\n"
#define HTTP_BAD    "HTTP/1.1 403 Forbidden\r\n"
#define XML_TICKET  "<ObtainTicketResponse><message></message><prolongationPeriod>%U" \
                    "</prolongationPeriod><responseCode>OK</responseCode><salt>%s" \
                    "</salt><ticketId>1</ticketId><ticketProperties>licensee=%s" \
                    "\tlicenseType=0\t" \
                    "</ticketProperties></ObtainTicketResponse>"
#define XML_PING    "<PingResponse><message></message><responseCode>OK</responseCode><salt>%s" \
                    "</salt></PingResponse>"
#define XML_RELEASE "<ReleaseTicketResponse><message></message><responseCode>OK</r" \
                    "esponseCode><salt>%s</salt></ReleaseTicketResponse>"
#define XML_PROLONG "<ProlongTicketResponse><message></message><responseCode>OK</r" \
                    "esponseCode><salt>%s</salt><ticketId>%s</ticketId></Prolong" \
                    "TicketResponse>"
#define FIELD_SALT  "salt="
#define FIELD_USER  "userName="
#define FIELD_TICK  "ticketId="
#define GETLEN(x)   (sizeof(x) - 1)
#define COTYPE_XML  1
#define COTYPE_HTML 2

static void reply_content(int fd, int is_ok, int co_type, sds* content)
{
    time_t timer;
    char buffer[BUFFER_SIZE];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, BUFFER_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
    
    const char *status = is_ok ? HTTP_OK : HTTP_BAD;
    const char *cotype;
    switch (co_type) {
        case COTYPE_XML:
          cotype = "application/xml";
          break;
        case COTYPE_HTML:
          cotype = "text/html";
          break;
        default:
          cotype = "text/plain";
    }
    
    sds reply;
    reply = sdsnew(status);
    reply = sdscat(reply, "Server: Jetty(9.2.10.v20150310)\r\n");
    reply = sdscatfmt(reply, "Date: %s\r\n", buffer);
    reply = sdscatfmt(reply, "Content-Type: %s\r\n", cotype);
    if (content != NULL) {
        reply = sdscatfmt(reply, "Content-Length: %i\r\n\r\n", sdslen(*content));
        reply = sdscatsds(reply, *content);
    } else {
        reply = sdscat(reply, "Content-Length: 0\r\n\r\n");
    }
#ifdef DEBUG
    fprintf(stderr, "RESPONSE >>>\n%s\n", reply);
#endif
    write(fd, reply, sdslen(reply));
    sdsfree(reply);
}

sds process_path_params(settings *setting, sds path, sds* params, int param_num,
                        int *is_ok, int *co_type)
{
    sds ret = sdsempty();
    char *pSalt = NULL;
    char *pUser = NULL;
    char *pTick = NULL;
    for (int i = 0; i < param_num; i++) {
        if (pSalt == NULL) {
            pSalt = strstr(params[i], FIELD_SALT);
            if (pSalt != NULL) {
                pSalt += GETLEN(FIELD_SALT);
                continue;
            }
        }
        if (pUser == NULL) {
            pUser = strstr(params[i], FIELD_USER);
            if (pUser != NULL) {
                pUser += GETLEN(FIELD_USER);
            }
        }
        if (pTick == NULL) {
            pTick = strstr(params[i], FIELD_TICK);
            if (pTick != NULL) {
                pTick += GETLEN(FIELD_TICK);
            }
        }
    }
    char *salt = pSalt ? pSalt : DEF_SALT;
    char *user = pUser ? pUser : DEF_USER;
    char *tick = pTick ? pTick : DEF_TICK;
    uint64_t prolong = DEF_PROLONG;
    int is_verbose = 0;
    if (setting) {
        if (setting->prolong > 0) {
            prolong = setting->prolong;
        }
        if (setting->licensee && pUser == NULL) {
            user = setting->licensee;
        }
        is_verbose = setting->verbose;
    }
    sds xml = NULL;
    if (strcmp(path, "/rpc/obtainTicket.action") == 0) {
        xml = sdscatfmt(sdsempty(), XML_TICKET, prolong, salt, user);
    } else if (strcmp(path, "/rpc/ping.action") == 0) {
        xml = sdscatfmt(sdsempty(), XML_PING, salt);
    } else if (strcmp(path, "/rpc/prolongTicket.action") == 0) {
        xml = sdscatfmt(sdsempty(), XML_PROLONG, salt, tick);
    } else if (strcmp(path, "/rpc/releaseTicket.action") == 0) {
        xml = sdscatfmt(sdsempty(), XML_RELEASE, salt);
    }
    if (xml != NULL) {
        sds sign = NULL;
        int sign_ret = rsa_sign_string(xml, &sign);
        if (sign_ret == 0) {
            *is_ok = 1;
            ret = sdscatfmt(ret, "<!-- %s -->\n%s", sign, xml);
            if (is_verbose) {
                LOG(INFO, "signed request for %s by %s", user, path);
            }
            sdsfree(sign);
        } else {
            *is_ok = 0;
            if (is_verbose) {
               LOG(ERROR, "sign message error (%d) by %s", sign_ret, path); 
            }
        }
        *co_type = COTYPE_XML;
        sdsfree(xml);
    }
    return ret;
}

void request_handler(char *input, size_t ilen, void *user, int fd)
{
    sds response = sdsempty();
    int is_ok = 0;
    int co_type = 0;
    int i;
    sds* headers;
    int header_num;
    sds inp = sdsnewlen(input, ilen);
    settings* userinfo = (settings *) user;
#ifdef DEBUG
    fprintf(stderr, "REQUEST <<<\n%s\n", inp);
#endif
    headers = sdssplitlen(inp, (int) sdslen(inp), "\r\n", 2, &header_num);
    for (i = 0; i < header_num; i++) {
        if (strstr(headers[i], "GET") != NULL) {
            int req_num;
            sds* reqs = sdssplitlen(headers[i], (int) sdslen(headers[i]), " ", 1, &req_num);
            if (req_num < 2) {
                goto FAIL1;
            }
            if (strcmp(reqs[1], "/") == 0 || strcmp(reqs[1], "/index.html") == 0) {
                if (userinfo && sdslen(userinfo->homepage) > 0) {
                    response = sdscatsds(response, userinfo->homepage);
                    is_ok = 1;
                    co_type = COTYPE_HTML;
                }
                goto FAIL1;
            }
            int path_num;
            sds* paths = sdssplitlen(reqs[1], (int) sdslen(reqs[1]), "?", 1, &path_num);
            if (path_num < 2) {
                goto FAIL2;
            }
            int param_num;
            sds* params = sdssplitlen(paths[1], (int) sdslen(paths[1]), "&", 1, &param_num);
            
            sds result = process_path_params(userinfo, paths[0], params, param_num,
                                             &is_ok, &co_type);
            response = sdscatsds(response, result);
            sdsfreesplitres(params, param_num);
            sdsfree(result);
FAIL2:
            sdsfreesplitres(paths, path_num);
FAIL1:
            sdsfreesplitres(reqs, req_num);
            break;
        }
    }
    reply_content(fd, is_ok, co_type, &response);
    close(fd);
    sdsfree(response);
    sdsfree(inp);
    sdsfreesplitres(headers, header_num);
}
