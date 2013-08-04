#include <stdio.h>
#include "tunnel.h"

#define TUNNEL_OPEN 0
#define TUNNEL_DATA 1
#define TUNNEL_CLOSE 2

void tunnel_init(void)
{
    // initialize udp socket
}

struct tunnel *tunnel_new(void)
{
    static uint32_t next_id = 0;

    struct tunnel *tunnel = calloc(1, sizeof(struct tunnel));
    tunnel->id = next_id++;

    tunnel_send_frame(tunnel, TUNNEL_OPEN, NULL, 0);

    return tunnel;
}

void tunnel_close(struct tunnel *tunnel)
{
    tunnel_send_frame(tunnel, TUNNEL_CLOSE, NULL, 0);

    free(tunnel);
}

void tunnel_send_frame(struct tunnel *tunnel, int code, char *data, size_t len)
{
    printf("tunnel_send_frame %d %zu\n", code, len);
}

void tunnel_send(struct tunnel *tunnel, char *data, size_t len)
{
    tunnel_send_frame(tunnel, TUNNEL_DATA, data, len);
}
