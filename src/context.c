
#include "ping_context.h"

extern ping_ctx_t ctx;

int init_ctx(const uint32_t socket_type, const uint32_t socket_protocol)
{
    bzero(&ctx, sizeof(ctx));

    // creating socket
	ctx.socket = socket(AF_INET, socket_type, socket_protocol);
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

void init_ctx_icmp_header(const uint32_t icmp_header_type, const pid_t pid)
{
    ctx.icmp_header.type = icmp_header_type;
    ctx.icmp_header.code = 0;
    ctx.icmp_header.checksum = 0;
    ctx.icmp_header.un.echo.id = htons (pid & 0xFFFF);
    ctx.icmp_header.un.echo.sequence = 0;
    ctx.icmp_header.checksum = checksum((uint16_t *)&ctx.icmp_header, sizeof(ctx.icmp_header));
}