#include <stdio.h>
#include "../sev/sev.h"
#include "tunnel.h"
#include "faketcp.h"

#define LOCAL_PORT 7777
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 7778

#define ADDRESS "127.0.0.1"
#define PORT 8080

void tunnel_open_cb(struct tunnel *tunnel)
{
    // never called
}

void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len)
{
    sev_send(tunnel->stream, data, len);
}

void tunnel_close_cb(struct tunnel *tunnel)
{
    sev_close(tunnel->stream, "remote_close");
}

void local_open_cb(struct sev_stream *stream)
{
    printf("open %s:%d\n", stream->remote_address, stream->remote_port);

    // start a new tunnel session

    struct tunnel *tunnel = tunnel_new();

    tunnel->stream = stream;
    tunnel->read_cb = tunnel_read_cb;
    tunnel->close_cb = tunnel_close_cb;

    stream->data = tunnel;
}

void local_read_cb(struct sev_stream *stream, char *data, size_t len)
{
    printf("read %s:%d\n", stream->remote_address, stream->remote_port);

    tunnel_send(stream->data, data, len);
}

void local_close_cb(struct sev_stream *stream, const char *reason)
{
    printf("close %s:%d %s\n",
        stream->remote_address, stream->remote_port, reason);

    tunnel_close(stream->data);
}

int main(int argc, char *argv[])
{
    if (faketcp_init(LOCAL_PORT, REMOTE_ADDRESS, REMOTE_PORT) == -1) {
        perror("tunnel_init");
        return -1;
    }

    printf("listening on udp:0.0.0.0:%d\n", LOCAL_PORT);

    struct sev_server server;
    if (sev_listen(&server, ADDRESS, PORT) == -1) {
        perror("sev_listen");
        return -1;
    }

    printf("listening on tcp:%s:%d\n", ADDRESS, PORT);

    server.open_cb = local_open_cb;
    server.read_cb = local_read_cb;
    server.close_cb = local_close_cb;

    sev_loop();

    return 0;
}
