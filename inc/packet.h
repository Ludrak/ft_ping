
#ifndef PACKET_H
#define PACKET_H

#include <netinet/in.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE  1024

typedef uint8_t  packet_t[MAX_PACKET_SIZE];

typedef struct  ping_packet
{
    // Headers
    struct iphdr   ip;
    struct icmphdr icmp;
    // Data
    struct timeval time;
}               ping_packet_t;

size_t  construct_ping_packet(ping_packet_t *const pk, const struct iphdr ip_header, const struct icmphdr icmp_header, const struct timeval time_data);
ssize_t construct_ping_packet_from_data(ping_packet_t *const pk, const void *const data, const size_t data_size);

struct icmphdr construct_ping_icmphdr(void);
struct iphdr   construct_ping_iphdr(const struct sockaddr_in dest_address);

int32_t checksum(const uint16_t *buff, const size_t size);

#endif