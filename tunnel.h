#ifndef TUNNEL_H
#define TUNNEL_H

#include "../sev/sev.h"

struct tunnel {
    uint32_t id;
    uint32_t last_sent;
    uint32_t last_received;

    void *data;

    void (*read_cb)(struct tunnel *tunnel, char *data, size_t len);
    void (*close_cb)(struct tunnel *tunnel);
};

void tunnel_init(void);
struct tunnel *tunnel_new(void);
void tunnel_close(struct tunnel *tunnel);
void tunnel_send_frame(struct tunnel *tunnel, int code, char *data, size_t len);
void tunnel_send(struct tunnel *tunnel, char *data, size_t len);

#endif
