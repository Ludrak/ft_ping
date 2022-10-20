
#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/icmp.h>
#include <signal.h>

/** checksum */
int32_t checksum(const uint16_t *buff, const size_t size);


#endif // PING_H