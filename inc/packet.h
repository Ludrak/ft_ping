
#ifndef PACKET_H
#define PACKET_H

#include <netinet/in.h>
#include <netdb.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>

#include <stdio.h>

#define MAX_PACKET_SIZE  1024

typedef uint8_t  packet_t[MAX_PACKET_SIZE];


typedef struct  ping_packet
{
    // Headers
    struct iphdr   ip;
    struct icmphdr icmp;
    // Time Data
    struct timeval time;

    // Data
    uint8_t        data[40];
} __attribute__((packed, aligned(4))) ping_packet_t;


size_t  construct_ping_packet(ping_packet_t *const pk, const struct iphdr ip_header, const struct icmphdr icmp_header);
ssize_t construct_ping_packet_from_data(ping_packet_t *const pk, const void *const data, const size_t data_size);

size_t  write_ping_packet_time(ping_packet_t *const pk);

int    get_packet_error(ping_packet_t pk, char *err_buffer);

struct icmphdr construct_ping_icmphdr(void);
struct iphdr   construct_ping_iphdr(const struct sockaddr_in dest_address);

uint16_t checksum(const uint16_t *buff, const size_t size);




#endif