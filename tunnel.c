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
#include "faketcp.h"
#include "tunnel.h"

extern void tunnel_open_cb(struct tunnel *tunnel);
extern void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len);
extern void tunnel_close_cb(struct tunnel *tunnel);

#define NUM_TUNNELS 256

static struct tunnel tunnels[NUM_TUNNELS] = {};
static uint8_t next_id = 0;

void faketcp_block_reads(void)
{
    for (int i=0; i<NUM_TUNNELS; i++) {
        if (tunnels[i].state == TUNNEL_ACTIVE && tunnels[i].stream)
            sev_block_read(tunnels[i].stream);
    }
}

void faketcp_allow_reads(void)
{
    for (int i=0; i<NUM_TUNNELS; i++) {
        if (tunnels[i].state == TUNNEL_ACTIVE && tunnels[i].stream)
            sev_allow_read(tunnels[i].stream);
    }
}

static int send_frame(struct tunnel *tunnel, int code, char *data, size_t len)
{
    printf("tunnel send %d %zu\n", code, len);

    char frame_data[HEADER_SIZE + PAYLOAD_SIZE];
    struct frame_header *header = (struct frame_header *)frame_data;

    header->code = code;
    header->id = tunnel->id;

    memcpy(frame_data + HEADER_SIZE, data, len);

    faketcp_send(frame_data, len + HEADER_SIZE);

    return 0;
}

static void tunnel_reset(struct tunnel *tunnel, uint8_t id)
{
    memset(tunnel, 0, sizeof(struct tunnel));
    tunnel->id = id;
    tunnel->state = TUNNEL_CLOSED;
    tunnel->stream = NULL;
}

void faketcp_recv(char *data, size_t len)
{
    struct frame_header *header = (struct frame_header *)data;

    printf("tunnel recv id %d code %d len %zu\n",
        header->id, header->code, len);

    struct tunnel *tunnel = &tunnels[header->id];

    if (header->code == TUNNEL_OPEN && tunnel->state == TUNNEL_CLOSED) {
        tunnel_reset(tunnel, header->id);
        tunnel->state = TUNNEL_ACTIVE;
        tunnel_open_cb(tunnel);
    }

    if (header->code == TUNNEL_DATA && tunnel->state == TUNNEL_ACTIVE)
        tunnel_read_cb(tunnel, data + HEADER_SIZE, len - HEADER_SIZE);

    if (header->code == TUNNEL_CLOSE && tunnel->state == TUNNEL_ACTIVE)
        tunnel_close_cb(tunnel);
}

struct tunnel *tunnel_new(void)
{
    while (tunnels[next_id % NUM_TUNNELS].state != TUNNEL_CLOSED)
        next_id++;

    struct tunnel *tunnel = &tunnels[next_id];
    tunnel_reset(tunnel, next_id++);
    send_frame(tunnel, TUNNEL_OPEN, NULL, 0);

    tunnel->state = TUNNEL_ACTIVE;

    return tunnel;
}

void tunnel_close(struct tunnel *tunnel)
{
    send_frame(tunnel, TUNNEL_CLOSE, NULL, 0);
    tunnel->state = TUNNEL_CLOSED;
    tunnel->stream = NULL;
}

void tunnel_send(struct tunnel *tunnel, char *data, size_t len)
{
    while (len > 0) {
        size_t num = len > PAYLOAD_SIZE ? PAYLOAD_SIZE : len;
        send_frame(tunnel, TUNNEL_DATA, data, num);

        data += num;
        len -= num;
    }
}
