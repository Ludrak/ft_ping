

#include "ping.h"
#include <linux/filter.h>

extern ping_ctx_t ctx;


static int set_socket_options(int socket, int options)
{
    // setting recv buffer size 
    int recv_buffer_size = 0x400;
    if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &recv_buffer_size, sizeof(recv_buffer_size)) != 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(SO_RCVBUF)", errno);
        return (errno);
    }

    // setting option to include iphdr when receiving
    int header_incl = 1;
    if (setsockopt(socket, IPPROTO_IP, IP_HDRINCL, &header_incl, sizeof(header_incl)) != 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(IP_HDRINCL)", errno);
        return (errno);
    }

    // setting icmp filters for filtering echo replies
    struct icmp_filter filter;
    filter.data = 0x1 << ICMP_ECHO;
    if (setsockopt(socket, SOL_RAW, ICMP_FILTER, &filter, sizeof(filter)))
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(ICMP_FILTER)", errno);
        return (errno);
    }

    // setting reuse port, so that packets will be sent to any process listening even on the same address/port
    int reuse_port = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &reuse_port, sizeof(reuse_port)) != 0)
    {
        if (options & OPT_VERBOSE)
            print_failed("setsockopt(SO_REUSEPORT)", errno);
        return (errno);
    }

    return (0);
}




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

    // setting options to the socket
    int err = set_socket_options(ctx.socket, options);
    if (err != 0)
    {
        return err; 
    }

    // resolving address from hostname
    ctx.sockaddr = resolve_address(hostname, options);
    if (ctx.sockaddr == NULL)
    {
        printf ("ft_ping: cannot resolve address\n");
        return (1);
    }

    // resolving hostname
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
