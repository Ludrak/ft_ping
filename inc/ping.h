
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
#include <sys/time.h>
#include <time.h>

#include "net_utils.h"
#include "print_utils.h"
#include "packet.h"

typedef struct  s_ping_ctx
{
    socket_t    socket;
    struct sockaddr_in *sockaddr;

    ping_packet_t packet;
}               ping_ctx_t;

extern ping_ctx_t ctx;

int  init_ctx(const string_hostname_t hostname);
void destroy_ctx();
#endif //PING_H