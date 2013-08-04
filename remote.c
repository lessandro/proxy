#include <stdio.h>
#include "../sev/sev.h"
#include "tunnel.h"

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
    tunnel_init();

    return 0;
}
