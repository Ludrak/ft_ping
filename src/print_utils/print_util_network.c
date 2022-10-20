#include "print_utils.h"


#define MAX_MSG_CONTROL_SZ 2048
#define MAX_MSG_IOV_SZ     2048
#define MAX_MSG_NAME_SZ    1024
void    print_msghdr(const struct msghdr header)
{
    char   msg_control[MAX_MSG_NAME_SZ];
    bzero(msg_control, MAX_MSG_NAME_SZ);

    char   msg_iov[MAX_MSG_NAME_SZ];
    size_t msg_iovlen = 0;
    bzero(msg_iov, MAX_MSG_NAME_SZ);

    char   msg_name[MAX_MSG_NAME_SZ];
    bzero(msg_name, MAX_MSG_NAME_SZ);

    // check for msg_control
    if (header.msg_controllen > 0 && header.msg_control)
    {
        strncpy(msg_control, header.msg_control, header.msg_controllen);
    }

    // check for msg_name
    if (header.msg_iovlen > 0 && header.msg_iov && header.msg_iov->iov_len > 0 && header.msg_iov->iov_base)
    {
        msg_iovlen = header.msg_iov->iov_len;
        strncpy(msg_name, header.msg_iov->iov_base, header.msg_iov->iov_len);
    }

    // check for msg_name
    if (header.msg_namelen > 0 && header.msg_name)
    {
        strncpy(msg_name, header.msg_name, header.msg_namelen);
    }

    printf("struct msghdr\n{\n    msg_control = \"%s\"\n    msg_controllen = %zu\n    msg_flags = %d\n    msg_iov->iov_base = \"%s\"\n    msg_iov->iovlen = %zu\n    msg_iovlen = %zu\n    msg_name = \"%s\"\n    msg_namelen = %d\n}\n",
    msg_control, header.msg_controllen, header.msg_flags, msg_iov, msg_iovlen, header.msg_iovlen, msg_name, header.msg_namelen);
}


void    print_icmphdr(const struct icmphdr header)
{
    printf ("struct icmphdr\n{\n    type = %d;\n    code = %d;\n    checksum = %d;\n    un.echo.id = %d;\n    un.echo.sequence = %d;\n    un.gateway = %d;\n    un.frag.mtu = %d;\n}\n",
    (int32_t)header.type, (int32_t)header.code, (int32_t)header.checksum, (int32_t)header.un.echo.id, (int32_t)header.un.echo.sequence, (int32_t)header.un.gateway, (int32_t)header.un.frag.mtu);
}


void    print_sockaddr_in(const struct sockaddr_in sockaddr)
{
    printf ("struct sockaddr_in\n{\n    sin_family = %d;\n    sin_port = %d;\n    sin_addr.s_addr = %#x\n    sin_zero = '%s'\n}\n",
    (uint32_t)sockaddr.sin_family, (uint32_t)sockaddr.sin_port, (uint32_t)sockaddr.sin_addr.s_addr, sockaddr.sin_zero);
}
