
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
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "time_utils.h"
#include "net_utils.h"
#include "print_utils.h"
#include "packet.h"
#include "options.h"

typedef struct s_ping_stats
{
    string_hostname_t  host_addr;

    uint16_t   n_packet_send;
    uint16_t   n_packet_recv;

    time_list_t  *packets_rtt;
}              ping_stats_t;


typedef struct  s_ping_ctx
{
    socket_t    socket;
    struct sockaddr_in *sockaddr;

    ping_packet_t packet;

    ping_stats_t  stats;
}               ping_ctx_t;




extern ping_ctx_t ctx;

int  init_ctx(const string_hostname_t hostname, int options);
int  ctx_add_package_stat(const time_t rtt);
void destroy_ctx();

#endif //PING_H