#include <stdio.h>
#include <string.h>
#include "sev/sev_udp.h"
#include "faketcp.h"

struct packet {
    int64_t seq;
    int state;
    struct ev_timer timer;
    char *data;
    int len;
};

#define MAX_PACKETS 8192

static struct packet sent[MAX_PACKETS];
static struct packet received[MAX_PACKETS];

static int64_t next_seq;
static int64_t expect_seq;

static struct sev_udp *udp;
static struct sev_addr remote_addr;

static void retransmit(struct packet *packet);

static void timer_cb(EV_P_ struct ev_timer *timer, int revents)
{
    retransmit(timer->data);
}

static void retransmit(struct packet *packet)
{
    printf("retransmitting seq %lld state %d\n", packet->seq, packet->state);

    sev_udp_sendto(udp, packet->data, packet->len, &remote_addr);

    double delay = (1 << (packet->state++ - 1)) * 0.1;

    ev_timer_init(&packet->timer, timer_cb, delay, 0);
    packet->timer.data = packet;
    ev_timer_start(EV_DEFAULT_ &packet->timer);
}

void faketcp_send(char *data, size_t len)
{
    printf("faketcp_send seq %lld len %zu\n", next_seq, len);
    struct packet *packet = &sent[next_seq % MAX_PACKETS];

    if (packet->state != 0) {
        printf("packet->stage != 0\n");
        return;
    }

    packet->seq = next_seq++;
    packet->state = 1;
    packet->len = sizeof(int64_t) + len;
    packet->data = malloc(packet->len);

    *(int64_t *)packet->data = packet->seq;
    memcpy(packet->data + sizeof(int64_t), data, len);

    retransmit(packet);
}

static void handle_ack(int64_t seq)
{
    struct packet *packet = &sent[seq % MAX_PACKETS];

    if (packet->seq != seq)
        return;

    if (packet->state == 0)
        return;

    ev_timer_stop(EV_DEFAULT_ &packet->timer);
    packet->state = 0;
    free(packet->data);
}

static void send_ack(int64_t seq)
{
    int64_t ack = -seq;
    sev_udp_sendto(udp, (char *)&ack, sizeof(int64_t), &remote_addr);
}

static void flush_received()
{
    for (;;) {
        struct packet *packet = &received[expect_seq % MAX_PACKETS];

        if (!packet->state || packet->seq != expect_seq)
            break;

        faketcp_recv(packet->data, packet->len);
        packet->state = 0;
        free(packet->data);
        expect_seq++;
    }
}

static void recv_data(struct sev_udp *udp, char *data, size_t len,
    struct sev_addr *addr)
{
    int64_t seq = *(int64_t *)data;

    printf("fakectp recv seq %lld len %zu\n", seq, len);

    if (seq < 0) {
        handle_ack(-seq);
        return;
    }

    send_ack(seq);

    data += sizeof(int64_t);
    len -= sizeof(int64_t);

    if (seq == expect_seq) {
        // in order
        faketcp_recv(data, len);
        expect_seq++;

        flush_received();
        return;
    }

    // out of order, store packet
    struct packet *packet = &received[seq % MAX_PACKETS];
    packet->seq = seq;
    packet->state = 1;
    packet->data = malloc(len);
    memcpy(packet->data, data, len);
    packet->len = len;
}

int faketcp_init(int local_port, const char *remote_address, int remote_port)
{
    udp = sev_udp_bind("0.0.0.0", local_port);
    if (!udp)
        return -1;

    udp->read_cb = recv_data;

    if (sev_addr_set(&remote_addr, remote_address, remote_port) == -1)
        return -1;

    memset(sent, 0, sizeof(sent));
    memset(received, 0, sizeof(received));
    next_seq = 1;
    expect_seq = 1;

    return 0;
}
