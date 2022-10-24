
#include "ping.h"
#include "time_utils.h"

ping_ctx_t ctx;

void on_interrupt(int sig)
{
    printf("Interrupted. (%d)\n", sig);
    destroy_ctx();
    exit (sig);
}

size_t construct_packet()
{
    struct iphdr   ip_header = construct_ping_iphdr(*ctx.sockaddr);
    struct icmphdr icmp_header = construct_ping_icmphdr();
    struct timeval time;

    gettimeofday(&time, NULL);
    return (construct_ping_packet(&ctx.packet, ip_header, icmp_header, time));
}

void on_alarm(int sig)
{
    (void)sig;
    size_t packet_size = construct_packet();
    // print_struct_iphdr(ctx.packet.ip);
    // print_struct_icmphdr(ctx.packet.icmp);
    // print_struct_timeval(ctx.packet.time);
    ssize_t bytes = sendto(ctx.socket, &ctx.packet, packet_size, 0, SOCKADDR(ctx.sockaddr), sizeof(*ctx.sockaddr));
    if (bytes < 0)
    {
        print_failed("sendto()", errno);
        return ;
    }
    // printf ("Send of %zd bytes successfull\n", bytes);

}



int main(int ac, char **av)
{
    if (ac < 2)
    {
        fprintf (stderr, "not enought parameters, expected: ft_ping <address>\n");
        return (1);
    }

    if (getuid() != 0)
    {
        fprintf (stderr, "this program must be run as root\n");
        return (1);
    }

    string_hostname_t ping_hostname = av[1];

    // setting up signals handlers
    signal(SIGALRM, on_alarm);
    signal(SIGINT, on_interrupt);

    // initialize ctx (socket and address)
    int init_err = init_ctx(ping_hostname);
    if (init_err != 0)
        return (init_err);
    
    string_hostname_t resolved_host = resolve_hostname(ctx.sockaddr);
    printf ("PING %s (%s)\n", ping_hostname, resolved_host);
    
    // print_struct_sockaddr_in(*ctx.sockaddr);

    // sending ping through alarm handler
    alarm(1);
   


    // recieve loop
    while (1)
    {
        uint8_t iov_base[0x200];
        bzero(iov_base, 0x200);

        struct msghdr  recv_message;
        bzero(&recv_message, sizeof(recv_message));
        recv_message.msg_name =  (void*)(struct sockaddr_in[]){};
        recv_message.msg_namelen = sizeof(struct sockaddr_in);
        recv_message.msg_iov = (struct iovec[]){{
            .iov_base = iov_base,
            .iov_len = 0x200
        }};
        recv_message.msg_iovlen = 1;
        recv_message.msg_control = (uint8_t[0x200]){};
        recv_message.msg_controllen = 0x200;
        recv_message.msg_flags = 0;

        ssize_t recieved_bytes = recvmsg(ctx.socket, &recv_message, MSG_CONFIRM); 
        if (recieved_bytes < 0)
        {
            print_failed("recvmsg()", errno);
            destroy_ctx();
            return 1;
        }
        struct timeval now;
        gettimeofday(&now, NULL);

        // print_struct_msghdr(recv_packet);

        ping_packet_t received_packet;
        construct_ping_packet_from_data(&received_packet, recv_message.msg_iov->iov_base, sizeof(ping_packet_t));
        // print_struct_iphdr(received_packet.ip);
        // print_struct_icmphdr(received_packet.icmp);
        // print_struct_timeval(ctx.packet.time);

        char *received_hostname = resolve_address_from_int(ctx.sockaddr->sin_family, received_packet.ip.saddr);
        printf ("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.3fms\n",
                        recieved_bytes,
                        received_hostname,
                        received_packet.icmp.un.echo.sequence,
                        received_packet.ip.ttl,
                        (float)(get_difference_time(ctx.packet.time, now)) / 1000.0f);
        free (received_hostname);

        // sending next ping through alarm handler
        alarm(1);
    }
}
