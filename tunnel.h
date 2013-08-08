#ifndef TUNNEL_H
#define TUNNEL_H

#include <stdlib.h>
#include <stdint.h>
#include <ev.h>

#define TUNNEL_OPEN 0
#define TUNNEL_DATA 1
#define TUNNEL_CLOSE 2

#define TUNNEL_CLOSED 0
#define TUNNEL_ACTIVE 1
#define TUNNEL_CLOSING 2

struct __attribute__((__packed__)) frame_header {
    uint8_t id;
    uint8_t code;
};

#define HEADER_SIZE sizeof(struct frame_header)
#define PAYLOAD_SIZE 1300

struct tunnel {
    uint8_t id;
    int state;

    void *data;

    void (*read_cb)(struct tunnel *tunnel, char *data, size_t len);
    void (*close_cb)(struct tunnel *tunnel);
};

struct tunnel *tunnel_new(void);
void tunnel_close(struct tunnel *tunnel);
void tunnel_send(struct tunnel *tunnel, char *data, size_t len);

#endif
