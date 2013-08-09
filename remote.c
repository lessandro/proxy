/*-
 * Copyright (c) 2013, Lessandro Mariano
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include "../sev/sev.h"
#include "tunnel.h"
#include "faketcp.h"

#define LOCAL_PORT 7778
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 7777


void tunnel_open_cb(struct tunnel *tunnel)
{
    // create new tunnel
}

void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len)
{
    // implement the http proxy protocol
    data[len] = '\0';
    printf("received: %s", data);

    tunnel_send(tunnel, data, strlen(data));
}

void tunnel_close_cb(struct tunnel *tunnel)
{
    // implement the http proxy protocol
}

int main(int argc, char *argv[])
{
    if (faketcp_init(LOCAL_PORT, REMOTE_ADDRESS, REMOTE_PORT) == -1) {
        perror("tunnel_init");
        return -1;
    }

    printf("listening on udp:0.0.0.0:%d\n", LOCAL_PORT);

    sev_loop();

    return 0;
}
