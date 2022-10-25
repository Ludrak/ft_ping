
#include "ping.h"

extern ping_ctx_t ctx;

int init_ctx(const string_hostname_t hostname)
{
    bzero(&ctx, sizeof(ctx));

    // creating socket
	ctx.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ctx.socket <= 0)
       return (print_failed("socket()", errno));

    // setting opt ?
    int send_buffer_size = 98304;
    if (setsockopt(ctx.socket, SOL_SOCKET, SO_RCVBUF, &send_buffer_size, sizeof(send_buffer_size)) != 0)
        return (print_failed("setsockopt(SO_RCVBUF)", errno));

    int header_incl = 1;
    if (setsockopt(ctx.socket, IPPROTO_IP, IP_HDRINCL, &header_incl, sizeof(header_incl)) != 0)
        return (print_failed("setsockopt(IP_HDRINCL)", errno));
    
    ctx.sockaddr = resolve_address(hostname);
    if (ctx.sockaddr == NULL)
        return (1);
    
    ctx.stats.host_addr = resolve_hostname(*ctx.sockaddr);
    ctx.stats.n_packet_send = 0;
    ctx.stats.n_packet_recv = 0;
    ctx.stats.packets_rtt = NULL;
    return (0);
}

int ctx_add_package_stat(const time_t rtt)
{
    ++ctx.stats.n_packet_recv;
    if (ctx.stats.packets_rtt == NULL)
    {
        ctx.stats.packets_rtt = malloc(sizeof(time_list_t));
        if (ctx.stats.packets_rtt == NULL)
            return (1);
        ctx.stats.packets_rtt->time = rtt;
        ctx.stats.packets_rtt->next = NULL;
    }
    else
    {
        time_list_t *n = ctx.stats.packets_rtt;
        while (n->next)
            n = n->next;
        n->next = malloc(sizeof(time_list_t));
        if (n->next == NULL)
            return (1);
        n->next->time = rtt;
        n->next->next = NULL;
    }
    return (0);
}

void destroy_ctx()
{
    free(ctx.sockaddr);
    free (ctx.stats.host_addr);
}
