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

#ifndef JBLS_C_REQUEST_H
#define JBLS_C_REQUEST_H

#include "sds.h"

typedef struct {
    char* licensee;
    uint64_t prolong;
    sds homepage;
    int verbose;
} settings;

void request_handler(char *input, size_t ilen, void *user, int fd);

#endif
