#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sev_udp.h"

#define RECV_BUFFER_SIZE 2048

int sev_addr_set(struct sev_addr *addr, const char *address, int port)
{
    memset(addr, 0, sizeof(struct sev_addr));
    struct sockaddr_in *s_in = (struct sockaddr_in *)&addr->addr;

    s_in->sin_family = AF_INET;
    s_in->sin_port = htons(port);

    if (inet_pton(AF_INET, address, &s_in->sin_addr) != 1)
        return -1;

    addr->addr_len = sizeof(struct sockaddr_in);

    return 0;
}

static void read_cb(EV_P_ struct ev_io *watcher, int revents)
{
    if (!(revents & EV_READ)) {
        // TODO: handle error
        return;
    }

    static char buffer[RECV_BUFFER_SIZE];

    struct sev_udp *udp = watcher->data;
    struct sev_addr addr;

    ssize_t n = recvfrom(udp->sd, buffer, RECV_BUFFER_SIZE - 1, 0,
        &addr.addr, &addr.addr_len);

    if (n == -1) {
        // TODO: handle error
        return;
    }

    if (udp->read_cb)
        udp->read_cb(udp, buffer, n, &addr);
}

struct sev_udp *sev_udp_bind(const char *address, int port)
{
    int sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd == -1)
        return NULL;

    struct sev_addr addr;
    if (sev_addr_set(&addr, address, port) == -1)
        return NULL;

    if (bind(sd, &addr.addr, addr.addr_len) == -1)
        return NULL;

    struct sev_udp *udp = calloc(1, sizeof(struct sev_udp));
    udp->sd = sd;

    ev_io_init(&udp->watcher, read_cb, sd, EV_READ);
    udp->watcher.data = udp;
    ev_io_start(EV_DEFAULT_ &udp->watcher);

    return udp;
}

int sev_udp_sendto(struct sev_udp *udp, const char *data, size_t len,
    struct sev_addr *addr)
{
    return sendto(udp->sd, data, len, 0, &addr->addr, addr->addr_len);
}
