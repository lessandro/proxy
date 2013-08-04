#include <stdio.h>
#include "../sev/sev.h"
#include "tunnel.h"

#define LOCAL_PORT 7778
#define REMOTE_ADDRESS "127.0.0.1"
#define REMOTE_PORT 7777

void tunnel_open_cb()
{
    // create new tunnel
}

void tunnel_read_cb()
{
    // implement the http proxy protocol
}

void tunnel_close_cb()
{
    // close stream
}

int main(int argc, char *argv[])
{
    if (tunnel_init(LOCAL_PORT, REMOTE_ADDRESS, REMOTE_PORT) == -1) {
        perror("tunnel_init");
        return -1;
    }

    printf("listening on udp:0.0.0.0:%d\n", LOCAL_PORT);

    sev_loop();

    return 0;
}
