
#ifndef PING_H
#define PING_H


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <signal.h>
#include "net_types.h"
#include "print_utils.h"
#include "packet.h"

typedef struct  s_ping_ctx
{
    socket_t    socket;
    struct sockaddr_in sockaddr;

    packet_t    packet;
}               ping_ctx_t;

extern ping_ctx_t ctx;

int  init_ctx(void);
void init_ctx_addr(const sa_family_t family, const string_address_t address);

#endif //PING_H