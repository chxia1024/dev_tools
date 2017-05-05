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

#include "signer.h"
#include "rsa.h"
#include "md5.h"
#include "bignum.h"
#include <stdio.h>
#include <stdlib.h>

#define KEY_RADIX 16
#define KEY_LEN 64
#define KEY_P "cb09437c6225e9a7276a43eb98913420bf2abee3b716db0706" \
              "ba6452e3a75da9"
#define KEY_Q "e782429f8d5f17c199f746fd2f976d150076ff221c197e17a3" \
              "b6c2be5f0368f3"
#define KEY_N "b79cab7070008e18259c2127eb279c3163de51770cca8d3e8a" \
              "d5eb2e592970a208caa3f0e78e9b19386195bd69a57d72497f" \
              "ce500e2349a9077a434fe5e58f6b"
#define KEY_E "10001"
#define KEY_D "5bafda543d39a9bb191ccbd9b92d806b916934d8f404395fdf" \
              "c84eb64843cf3fe19186fe10160b2ef95d860b03d0ebd1372f" \
              "32873ec48da52a2c20748ee20de1"

typedef struct {
    uint8_t *data;
    size_t len;
} bytes;

static sds bytes_to_sds(bytes *input)
{
    sds result = sdsempty();
    for (int i = 0; i < input->len; i++) {
        result = sdscatprintf(result, "%02x", input->data[i]);
    }
    return result;
}

static int rsa_signer(bytes message, bytes *sig)
{
    rsa_context ctx;
    bytes result;
    uint8_t hash[16];
    int ret;
    
    if (sig == NULL) {
        return -1;
    }
    
    rsa_init(&ctx);
    mpi_init(&ctx.P);
    mpi_init(&ctx.Q);
    mpi_init(&ctx.N);
    mpi_init(&ctx.E);
    mpi_init(&ctx.D);
    mpi_read_string(&ctx.P, KEY_RADIX, KEY_P);
    mpi_read_string(&ctx.Q, KEY_RADIX, KEY_Q);
    mpi_read_string(&ctx.N, KEY_RADIX, KEY_N);
    mpi_read_string(&ctx.E, KEY_RADIX, KEY_E);
    mpi_read_string(&ctx.D, KEY_RADIX, KEY_D);
    ctx.len = KEY_LEN;
    
    md5(message.data, message.len, hash);
    result.len = ctx.len;
    result.data = (uint8_t *) malloc(ctx.len);
    ret = rsa_sign(&ctx, hash, result.data);
    if (ret == 0) {
        *sig = result;
    } else {
        free(result.data);
    }
    rsa_free(&ctx);
    return ret;
}

int rsa_sign_string(sds message, sds* sig)
{
    bytes input, output;
    int ret;
    if (message == NULL || sig == NULL) {
        return -1;
    }
    if (sdslen(message) == 0) {
        return -1;
    }
    input.len = sdslen(message);
    input.data = (uint8_t *) message;
    ret = rsa_signer(input, &output);
    if (ret == 0) {
        *sig = bytes_to_sds(&output);
        free(output.data);
    }
    return ret;
}
