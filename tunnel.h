#ifndef TUNNEL_H
#define TUNNEL_H

#include "../sev/sev.h"

struct tunnel {
    int alive;
    uint8_t id;
    int32_t last_sent;
    int32_t last_received;
    int32_t last_ackd;

    void *data;

    void (*read_cb)(struct tunnel *tunnel, char *data, size_t len);
    void (*close_cb)(struct tunnel *tunnel);
};

int tunnel_init(int local_port, const char *remote_address, int remote_port);
void tunnel_reset(struct tunnel *tunnel, uint8_t id);
struct tunnel *tunnel_new(void);
void tunnel_close(struct tunnel *tunnel);
void tunnel_send(struct tunnel *tunnel, char *data, size_t len);

#endif
