
#include "packet.h"

void    construct_packet(packet_t pk, const struct iphdr ip_header, const struct icmphdr icmp_header)
{
    bzero(pk, MAX_PACKET_SIZE);
    memcpy(pk, (uint8_t*)&ip_header, sizeof(ip_header));
    memcpy(pk + sizeof(ip_header), (uint8_t*)&icmp_header, sizeof(icmp_header));
}

struct icmphdr construct_ping_icmphdr(void)
{
    struct icmphdr header = (struct icmphdr){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo.id = htons (getpid() & 0xFFFF),
        .un.echo.sequence = 0,
    };
    header.checksum = checksum((uint16_t *)&header, sizeof(header));
    return (header);
}


struct iphdr construct_ping_iphdr(const struct sockaddr_in sock_address)
{
    return ((struct iphdr){
        .version = 4,
        .ihl = 5,
        .tos = 0,
        .tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr),
        .id = 0,
        .frag_off = 0,
        .ttl = 0X40,
        .protocol = IPPROTO_ICMP,
        .check = 0,
        .saddr = INADDR_ANY,
        .daddr = sock_address.sin_addr.s_addr
    });
}