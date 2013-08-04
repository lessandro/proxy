#ifndef SEV_UDP_H
#define SEV_UDP_H

#include <stdlib.h>
#include <arpa/inet.h>
#include <ev.h>

struct sev_addr
{
    struct sockaddr addr;
    socklen_t addr_len;
};

struct sev_udp
{
    int sd;
    struct ev_io watcher;

    void *data;

    void (*read_cb)(struct sev_udp *, char *data, size_t len,
        struct sev_addr *addr);
};

int sev_addr_set(struct sev_addr *addr, const char *address, int port);

struct sev_udp *sev_udp_bind(const char *address, int port);

int sev_udp_sendto(struct sev_udp *udp, const char *data, size_t len,
    struct sev_addr *addr);

#endif
