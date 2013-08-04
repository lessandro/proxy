#include <stdio.h>
#include <string.h>
#include <ev.h>
#include "tunnel.h"
#include "sev_udp.h"

#define TUNNEL_OPEN 0
#define TUNNEL_DATA 1
#define TUNNEL_CLOSE 2

struct sev_udp *udp;
struct sev_addr remote_addr;

void read_cb(struct sev_udp *udp, char *data, size_t len, struct sev_addr *addr)
{
    data[len] = '\0';
    if (data[len-1] == '\n')
        data[len-1] = 0;

    printf("udp: %s\n", data);
}

int tunnel_init(int local_port, const char *remote_address, int remote_port)
{
    udp = sev_udp_bind("0.0.0.0", local_port);
    if (!udp)
        return -1;

    udp->read_cb = read_cb;

    if (sev_addr_set(&remote_addr, remote_address, remote_port) == -1)
        return -1;

    sev_udp_sendto(udp, "hello\n", 6, &remote_addr);

    return 0;
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
