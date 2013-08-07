#include <stdio.h>
#include <string.h>
#include "tunnel.h"
#include "sev/sev_udp.h"

#define TUNNEL_OPEN 0
#define TUNNEL_DATA 1
#define TUNNEL_CLOSE 2
#define TUNNEL_ACK 3

extern void tunnel_open_cb(struct tunnel *tunnel);
extern void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len);
extern void tunnel_close_cb(struct tunnel *tunnel);

struct __attribute__((__packed__)) frame_header {
    uint8_t id;
    uint8_t code;
    int32_t seq;
};

static struct tunnel tunnels[256];
static uint8_t next_id = 0;
static struct sev_udp *udp;
static struct sev_addr remote_addr;

static void send_frame(struct tunnel *tunnel, int code, char *data, size_t len)
{
    printf("send_frame %d %zu\n", code, len);

    char buf[len + sizeof(struct frame_header)];
    struct frame_header header;

    header.id = tunnel->id;
    header.code = code;

    if (code == TUNNEL_ACK)
        header.seq = tunnel->last_received;
    else
        header.seq = ++tunnel->last_sent;

    memcpy(buf, &header, sizeof(struct frame_header));
    memcpy(buf + sizeof(struct frame_header), data, len);

    sev_udp_sendto(udp, buf, len + sizeof(struct frame_header), &remote_addr);
}

static void recv_frame(struct sev_udp *udp, char *data, size_t len,
    struct sev_addr *addr)
{
    struct frame_header header;

    memcpy(&header, data, sizeof(struct frame_header));

    printf("recv_frame: %d %d %d %zu\n", header.id, header.code, header.seq, len);

    struct tunnel *tunnel = &tunnels[header.id];

    if (!tunnel->alive && header.code != TUNNEL_OPEN) {
        // error
        return;
    }

    if (header.code == TUNNEL_ACK) {
        tunnel->last_ackd = header.seq;
        return;
    }

    send_frame(tunnel, TUNNEL_ACK, NULL, 0);

    tunnel->last_received = header.seq;

    if (header.code == TUNNEL_OPEN) {
        tunnel_reset(tunnel, header.id);
        tunnel_open_cb(tunnel);
    }

    if (header.code == TUNNEL_DATA)
        tunnel_read_cb(tunnel, data + sizeof(struct frame_header),
            len - sizeof(struct frame_header));

    if (header.code == TUNNEL_CLOSE)
        tunnel_close_cb(tunnel);
}

int tunnel_init(int local_port, const char *remote_address, int remote_port)
{
    udp = sev_udp_bind("0.0.0.0", local_port);
    if (!udp)
        return -1;

    udp->read_cb = recv_frame;

    if (sev_addr_set(&remote_addr, remote_address, remote_port) == -1)
        return -1;

    return 0;
}

void tunnel_reset(struct tunnel *tunnel, uint8_t id)
{
    memset(tunnel, 0, sizeof(struct tunnel));
    tunnel->alive = 1;
    tunnel->id = id;
    tunnel->last_received = -1;
    tunnel->last_sent = -1;
}

struct tunnel *tunnel_new(void)
{
    while (tunnels[next_id].alive)
        next_id++;

    struct tunnel *tunnel = &tunnels[next_id];

    tunnel_reset(tunnel, next_id++);

    send_frame(tunnel, TUNNEL_OPEN, NULL, 0);

    return tunnel;
}

void tunnel_close(struct tunnel *tunnel)
{
    send_frame(tunnel, TUNNEL_CLOSE, NULL, 0);
}

void tunnel_send(struct tunnel *tunnel, char *data, size_t len)
{
    send_frame(tunnel, TUNNEL_DATA, data, len);
}
