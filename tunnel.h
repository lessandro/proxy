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

#ifndef TUNNEL_H
#define TUNNEL_H

#include <stdlib.h>
#include <stdint.h>
#include <ev.h>
#include "../sev/sev.h"

#define TUNNEL_OPEN 0
#define TUNNEL_DATA 1
#define TUNNEL_CLOSE 2

#define TUNNEL_CLOSED 0
#define TUNNEL_ACTIVE 1

struct __attribute__((__packed__)) frame_header {
    uint8_t id;
    uint8_t code;
};

#define HEADER_SIZE sizeof(struct frame_header)
#define PAYLOAD_SIZE 1300

struct tunnel {
    uint8_t id;
    int state;

    struct sev_stream *stream;

    void (*read_cb)(struct tunnel *tunnel, char *data, size_t len);
    void (*close_cb)(struct tunnel *tunnel);
};

struct tunnel *tunnel_new(void);
void tunnel_close(struct tunnel *tunnel);
void tunnel_send(struct tunnel *tunnel, char *data, size_t len);

#endif
