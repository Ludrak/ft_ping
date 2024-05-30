
#include "ping.h"

extern ping_ctx_t ctx;

int init_ctx(const string_hostname_t hostname, int options)
{
    bzero(&ctx, sizeof(ctx));

    // creating socket
	ctx.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ctx.socket <= 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("socket()", errno);
        return (errno);
    }

    // setting opt ?
    int send_buffer_size = 0x400;
    if (setsockopt(ctx.socket, SOL_SOCKET, SO_RCVBUF, &send_buffer_size, sizeof(send_buffer_size)) != 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(SO_RCVBUF)", errno);
        return (errno);
    }

    int header_incl = 1;
    if (setsockopt(ctx.socket, IPPROTO_IP, IP_HDRINCL, &header_incl, sizeof(header_incl)) != 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(IP_HDRINCL)", errno);
        return (errno);
    }

 //   struct timeval timeout = { .tv_usec = 0, .tv_sec = 1 };
 //   setsockopt(ctx.socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ctx.sockaddr = resolve_address(hostname, options);
    if (ctx.sockaddr == NULL)
    {
        printf ("ft_ping: cannot resolve address\n");
        return (1);
    }

    ctx.stats.host_addr = resolve_hostname(*ctx.sockaddr, options);
    if (!ctx.stats.host_addr)
    {
        printf ("ft_ping: unkown host\n");
        return (1);
    }
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
    time_list_t* n = ctx.stats.packets_rtt;
    time_list_t* tmp;
    while (n)
    {
        tmp = n->next;
        free(n);
        n = tmp;
    }

    free(ctx.sockaddr);
    free (ctx.stats.host_addr);
}
