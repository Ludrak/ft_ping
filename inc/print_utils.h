
#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdio.h>
#include <stddef.h>
#include <linux/icmp.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <string.h>

/** Print errno */
int     print_failed(const char *const caller, const int32_t err);

/** Print binary */
void    print_binary_be(const void *const param, const size_t size);
void    print_binary_se(const void *const param, const size_t size);

/** Print network */
void    print_icmphdr(const struct icmphdr header);
void    print_iphdr(const struct iphdr header);
void    print_sockaddr_in(const struct sockaddr_in sockaddr);
void    print_msghdr(const struct msghdr header);

#endif // PRINT_UTILS_H