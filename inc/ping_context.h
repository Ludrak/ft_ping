
#ifndef PING_CONTEXT_H
#define PING_CONTEXT_H

#include "net_types.h"
#include "print_utils.h"
#include "ping.h"

typedef struct  s_ping_ctx
{
    socket_t    socket;
    struct sockaddr_in sockaddr;
    struct icmphdr     icmp_header;
}               ping_ctx_t;

extern ping_ctx_t ctx;

int  init_ctx(const uint32_t socket_type, const uint32_t socket_protocol);
void init_ctx_addr(const sa_family_t family, const string_address_t address);
void init_ctx_icmp_header(const uint32_t icmp_header_type, const pid_t pid);


#endif //PING_CONTEXT_H