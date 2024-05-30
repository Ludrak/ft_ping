
#include "ping.h"
#include "time_utils.h"
#include <math.h>

ping_ctx_t ctx;



void on_interrupt(int sig)
{
    printf("--- %s ping statistics ---\n", ctx.stats.host_addr);

    uint16_t packet_loss = 100;
    if (ctx.stats.n_packet_recv > 0)
        packet_loss = 100 - ((float)ctx.stats.n_packet_recv / (float)ctx.stats.n_packet_send) * 100;
    printf("%hd packets transmitted, %hd packets received, %hd%% packet loss\n", ctx.stats.n_packet_send, ctx.stats.n_packet_recv, packet_loss);
    float min_rtt = __FLT_MAX__;
    float max_rtt = -1;
    float avg_rtt = 0;
    float sqr_avg_rtt = 0;
    float stddev_rtt =  0; // TODO
    struct s_time_list* t = ctx.stats.packets_rtt;
    if (t != NULL)
    {
        while (t)
        {
            if (t->time < min_rtt)
                min_rtt = t->time;
            if (t->time > max_rtt)
                max_rtt = t->time;
            avg_rtt += (t->time / 1000.0f);
            sqr_avg_rtt += (t->time / 1000.0f) * (t->time / 1000.0f);
            t = t->next;
        }
        avg_rtt /= ctx.stats.n_packet_recv;
        sqr_avg_rtt /= ctx.stats.n_packet_recv;
        min_rtt /= 1000.0f;
        max_rtt /= 1000.0f;
        stddev_rtt = sqrtf(sqr_avg_rtt - (avg_rtt * avg_rtt));
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", min_rtt, avg_rtt, max_rtt, stddev_rtt);
    }
    destroy_ctx();
    exit (sig);
}




size_t  construct_packet()
{
    struct iphdr   ip_header = construct_ping_iphdr(*ctx.sockaddr);
    struct icmphdr icmp_header = construct_ping_icmphdr();

    return (construct_ping_packet(&ctx.packet, ip_header, icmp_header));
}




void on_alarm(int sig)
{
    alarm(1);

    (void)sig;
    construct_packet();
    write_ping_packet_time(&ctx.packet);

    memset(&ctx.packet.data, '*', sizeof(ctx.packet.data));
    ctx.packet.icmp.checksum = checksum((uint16_t*)&ctx.packet.icmp, sizeof(ctx.packet) - sizeof(struct iphdr));

    ssize_t bytes = sendto(ctx.socket, &ctx.packet, sizeof(ctx.packet), 0, SOCKADDR(ctx.sockaddr), sizeof(*ctx.sockaddr));
    if (bytes < 0)
    {
        print_failed("sendto()", errno);
        return ;
    }
    ++ctx.stats.n_packet_send;
}





int ping(string_hostname_t host, int options)
{

    // initialize ctx (socket and address)
    int init_err = init_ctx(host, options);
    if (init_err != 0)
        return (init_err);
    
    printf ("PING %s (%s): %lu data bytes\n", host, ctx.stats.host_addr, sizeof(ctx.packet) - sizeof(struct iphdr) - sizeof(struct icmphdr));

    // calling alarm handler for 1st packet send
    on_alarm(0);

    // recieve loop
    while (1)
    {
        // construct msg header
        uint8_t iov_base[0x200];
        bzero(iov_base, 0x200);

        struct msghdr  received_message;
        bzero(&received_message, sizeof(received_message));
        received_message.msg_name =  (void*)(struct sockaddr_in[]){};
        received_message.msg_namelen = sizeof(struct sockaddr_in);
        received_message.msg_iov = (struct iovec[]){{
            .iov_base = iov_base,
            .iov_len = 0x200
        }};
        received_message.msg_iovlen = 1;
        received_message.msg_control = (uint8_t[0x200]){};
        received_message.msg_controllen = 0x200;
        received_message.msg_flags = 0;

        // waiting for message
        ssize_t received_bytes = recvmsg(ctx.socket, &received_message, 0); 
        if (received_bytes < 0)
        {
            if (errno == EINTR)
            {
                continue ;
            }
            if (options & OPT_VERBOSE)
                print_failed("recvmsg()", errno);
            destroy_ctx();
            return 1;
        }

        // get recv time
        struct timeval now;
        gettimeofday(&now, NULL);

        // retrieve packet
        ping_packet_t received_packet;
        construct_ping_packet_from_data(&received_packet, received_message.msg_iov->iov_base, sizeof(ping_packet_t));

        // skip packets that are not emitted from this program
        if (ntohs(received_packet.icmp.un.echo.id) != (getpid() & 0xFFFF))
            continue;

        // skip echo request packets
        if (received_packet.icmp.type == ICMP_ECHO)
            continue ;

        // resolving host
        char *resolved_hostname = resolve_address_from_int(ctx.sockaddr->sin_family, received_packet.ip.saddr, options);

        // check for errors in packet
        char err_message[128];
        bzero(err_message, 128);
        int err = get_packet_error(received_packet, (char*)&err_message);
        if (err != 0)
        {
            printf ("%ld bytes from %s: %s\n",
                received_bytes - sizeof(struct iphdr),
                resolved_hostname,
                err_message);
            free(resolved_hostname);
            continue ;
        }

        // add time difference data
        time_t time_difference = get_difference_timeval(received_packet.time, now);
        ctx_add_package_stat(time_difference);
    
        // Print ping infos
        printf ("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.3fms\n",
            received_bytes - sizeof(struct iphdr),
            resolved_hostname,
            received_packet.icmp.un.echo.sequence, 
            received_packet.ip.ttl,
            (float)(time_difference) / 1000.0f);
        
        free (resolved_hostname);
    }

    return (0);
}






int main(int ac, char **av)
{
    if (getuid() != 0)
    {
        fprintf (stderr, "this program must be run as root\n");
        return (1);
    }

    if (ac < 2)
    {
        fprintf (stderr, "not enought parameters, expected: ft_ping [-v?] <address>\n");
        return (1);
    }

    int options = get_options(ac, av);
    if (options < 0)
        return (1);

    if (options & OPT_USAGE)
        return (print_usage(av[0]));

    // setting up signals handlers
    signal(SIGALRM, on_alarm);
    signal(SIGINT, on_interrupt);

    int host_idx = get_host_arg(ac, av);
    if (host_idx < 0)
    {
        fprintf(stderr, "%s: missing host operand\n", av[0]);
        fprintf(stderr, "Try '%s -?' for more informations\n", av[0]);
        return (1);
    }
    return (ping(av[host_idx], options));
}
