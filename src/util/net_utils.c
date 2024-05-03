
#include "net_utils.h"
#include "ping.h"

struct sockaddr_in  *resolve_address(const string_hostname_t host, int options)
{
    struct addrinfo *result = NULL;
	struct addrinfo hints = (struct addrinfo){
		.ai_flags = AI_CANONNAME,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
	};
    struct sockaddr  *address = NULL;

    int err = getaddrinfo(host, 0, &hints, &result);
    if (err != 0) {
        if (options & OPT_VERBOSE)
            fprintf(stderr, "getaddrinfo(): %s: %d: %s\n", host, err, gai_strerror(err));
        freeaddrinfo(result);
        return (NULL);
    }
    if (result != NULL)
    {
        address = malloc(sizeof(struct sockaddr_in));
        COPY_SOCKADDR(address, result->ai_addr);
        freeaddrinfo(result);
        return (SOCKADDR_IN(address));
    }
    freeaddrinfo(result);
    return (NULL);
}

string_hostname_t   resolve_hostname(const struct sockaddr_in address, int options)
{
    char host_buffer[MAX_HOSTNAME_SIZE_4];
    bzero(host_buffer, MAX_HOSTNAME_SIZE_4);
    if (inet_ntop(address.sin_family, &address.sin_addr.s_addr, host_buffer, MAX_HOSTNAME_SIZE_4) == NULL)
    {
        if (options & OPT_VERBOSE)
            print_failed("inet_ntop()", errno);
        return (NULL);
    }
    char *s_address = malloc(sizeof(char) * (strlen(host_buffer) + 1));
    if (!s_address)
    {
        return(NULL);
    }
    strcpy(s_address, host_buffer);
    return (s_address);
}

string_hostname_t   resolve_address_from_int(const sa_family_t address_family, const uint32_t address, int options)
{
    char host_buffer[MAX_HOSTNAME_SIZE_4];
    bzero(host_buffer, MAX_HOSTNAME_SIZE_4);
    if (inet_ntop(address_family, &address, host_buffer, MAX_HOSTNAME_SIZE_4) == NULL)
    {
        if (options & OPT_VERBOSE)
        print_failed("inet_ntop()", errno);
        return (NULL);
    }
    char *s_address = malloc(sizeof(char) * (strlen(host_buffer) + 1));
    if (!s_address)
    {
        return(NULL);
    }
    strcpy(s_address, host_buffer);
    return (s_address);
}