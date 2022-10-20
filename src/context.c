
#include "ping.h"

extern ping_ctx_t ctx;

int init_ctx(void)
{
    bzero(&ctx, sizeof(ctx));

    // creating socket
	ctx.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ctx.socket <= 0)
       return (print_failed("socket()", errno));

    int send_buffer_size = 98304;
    if (setsockopt(ctx.socket, SOL_SOCKET, SO_RCVBUF, &send_buffer_size, sizeof(send_buffer_size)) != 0)
        return (print_failed("setsockopt(SO_RCVBUF)", errno));
    return (0);
}

void init_ctx_addr(const sa_family_t family, const string_address_t address)
{
    ctx.sockaddr.sin_family = family;
    inet_aton(address, &ctx.sockaddr.sin_addr);
}
