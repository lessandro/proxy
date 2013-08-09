#ifndef FAKETCP_H
#define FAKETCP_H

#include <stdlib.h>

extern void faketcp_recv(char *data, size_t len);
extern void faketcp_block_reads(void);
extern void faketcp_allow_reads(void);

void faketcp_send(char *data, size_t len);

int faketcp_init(int local_port, const char *remote_address, int remote_port);

#endif
