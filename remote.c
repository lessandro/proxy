#include <stdio.h>
#include <string.h>
#include "../sev/sev.h"
#include "tunnel.h"
#include "faketcp.h"

#define LOCAL_PORT 7778
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 7777


void tunnel_open_cb(struct tunnel *tunnel)
{
    // create new tunnel
}

void tunnel_read_cb(struct tunnel *tunnel, char *data, size_t len)
{
    // implement the http proxy protocol
    data[len] = '\0';
    printf("received: %s", data);

    tunnel_send(tunnel, data, strlen(data));
}

void tunnel_close_cb(struct tunnel *tunnel)
{
    // implement the http proxy protocol
}

int main(int argc, char *argv[])
{
    if (faketcp_init(LOCAL_PORT, REMOTE_ADDRESS, REMOTE_PORT) == -1) {
        perror("tunnel_init");
        return -1;
    }

    printf("listening on udp:0.0.0.0:%d\n", LOCAL_PORT);

    sev_loop();

    return 0;
}
