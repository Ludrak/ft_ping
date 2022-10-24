
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
        
    ctx.sockaddr = resolve_address(hostname);
    if (ctx.sockaddr == NULL)
        return (1);
    return (0);
}

void destroy_ctx()
{
    free(ctx.sockaddr);
}
