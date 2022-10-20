
#ifndef PACKET_H
#define PACKET_H

#include <netinet/in.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>

#define MAX_PACKET_SIZE  1024

typedef uint8_t  packet_t[MAX_PACKET_SIZE];

void    construct_packet(packet_t pk, const struct iphdr ip_header, const struct icmphdr icmp_header);

struct icmphdr construct_ping_icmphdr(void);
struct iphdr   construct_ping_iphdr(const struct sockaddr_in sockaddr_in);

int32_t checksum(const uint16_t *buff, const size_t size);

#endif