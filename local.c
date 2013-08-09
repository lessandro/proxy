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
#include "../sev/sev.h"
#include "tunnel.h"
#include "faketcp.h"

#define LOCAL_PORT 7777
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 7778

#define ADDRESS "127.0.0.1"
#define PORT 8080

void tunnel_open_cb(struct tunnel *tunnel)
{
    // never called
}

void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len)
{
    sev_send(tunnel->stream, data, len);
}

void tunnel_close_cb(struct tunnel *tunnel)
{
    sev_close(tunnel->stream, "remote_close");
}

void local_open_cb(struct sev_stream *stream)
{
    printf("open %s:%d\n", stream->remote_address, stream->remote_port);

    // start a new tunnel session

    struct tunnel *tunnel = tunnel_new();

    tunnel->stream = stream;
    tunnel->read_cb = tunnel_read_cb;
    tunnel->close_cb = tunnel_close_cb;

    stream->data = tunnel;
}

void local_read_cb(struct sev_stream *stream, char *data, size_t len)
{
    printf("read %s:%d\n", stream->remote_address, stream->remote_port);

    tunnel_send(stream->data, data, len);
}

void local_close_cb(struct sev_stream *stream, const char *reason)
{
    printf("close %s:%d %s\n",
        stream->remote_address, stream->remote_port, reason);

    tunnel_close(stream->data);
}

int main(int argc, char *argv[])
{
    if (faketcp_init(LOCAL_PORT, REMOTE_ADDRESS, REMOTE_PORT) == -1) {
        perror("tunnel_init");
        return -1;
    }

    printf("listening on udp:0.0.0.0:%d\n", LOCAL_PORT);

    struct sev_server server;
    if (sev_listen(&server, ADDRESS, PORT) == -1) {
        perror("sev_listen");
        return -1;
    }

    printf("listening on tcp:%s:%d\n", ADDRESS, PORT);

    server.open_cb = local_open_cb;
    server.read_cb = local_read_cb;
    server.close_cb = local_close_cb;

    sev_loop();

    return 0;
}
