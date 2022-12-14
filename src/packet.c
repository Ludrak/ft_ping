
#include "packet.h"

size_t   construct_ping_packet(ping_packet_t *const pk, const struct iphdr ip_header, const struct icmphdr icmp_header)
{
    bzero(pk, sizeof(*pk));
    memcpy(&pk->ip, (void *)&ip_header, sizeof(struct iphdr));
    memcpy(&pk->icmp, (void *)&icmp_header, sizeof(struct icmphdr));
    return (sizeof(*pk));
}

size_t   write_ping_packet_time(ping_packet_t *const pk)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    memcpy(&pk->time, (void *)&tv, sizeof(struct timeval));
    return (sizeof(*pk));
}

ssize_t  construct_ping_packet_from_data(ping_packet_t *const pk, const void *const data, const size_t data_size)
{
    if (data_size > sizeof(ping_packet_t))
        return (sizeof(ping_packet_t) - data_size);
    memcpy(pk, data, data_size);
    return (0);
}

struct iphdr construct_ping_iphdr(const struct sockaddr_in dest_address)
{
    struct iphdr header = (struct iphdr){
        .version = 4,
        .ihl = 5,
        .tos = 0,
        .tot_len = sizeof(ping_packet_t),
        .id = 0,

        // [0]RESERVED [1]MF [2]DF [.*13]fragmentss count
        .frag_off = 0,
        .ttl = 64,
        .protocol = IPPROTO_ICMP,
        .check = 0,
        .saddr = INADDR_ANY,
        .daddr = dest_address.sin_addr.s_addr
    };
    header.check = checksum((uint16_t *)&header, sizeof(header));
    return (header);
}

struct icmphdr construct_ping_icmphdr(void)
{
    static int sequence_count = 0;
    struct icmphdr header = (struct icmphdr){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo.id = htons(getpid() & 0xFFFF),
        .un.echo.sequence = sequence_count,
    };
    ++sequence_count;
    header.checksum = checksum((uint16_t *)&header, sizeof(header));
    return (header);
}
