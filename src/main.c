
#include "ping.h"
#include "time_utils.h"

ping_ctx_t ctx;

void on_interrupt(int sig)
{
    printf("--- %s ping statistics ---\n", ctx.stats.host_addr);

    unsigned short packet_loss = 100;
    if (ctx.stats.n_packet_recv > 0)
        packet_loss -= (ctx.stats.n_packet_send * 100 / ctx.stats.n_packet_recv);
    printf("%hd packets transmitted, %hd packets received, %hd%% packet loss\n", ctx.stats.n_packet_send, ctx.stats.n_packet_recv, packet_loss);
    float min_rtt = __FLT_MAX__;
    float max_rtt = -1;
    float avg_rtt = 0;
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
            t = t->next;
        }
        avg_rtt /= ctx.stats.n_packet_recv;
        min_rtt /= 1000.0f;
        max_rtt /= 1000.0f;
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
    (void)sig;
    construct_packet();
    write_ping_packet_time(&ctx.packet);

    // printf ("sending ping of %lu bytes\n", sizeof(ctx.packet));
    ssize_t bytes = sendto(ctx.socket, &ctx.packet, sizeof(ctx.packet), 0, SOCKADDR(ctx.sockaddr), sizeof(*ctx.sockaddr));
    if (bytes < 0)
    {
        print_failed("sendto()", errno);
        return ;
    }
    ++ctx.stats.n_packet_send;
}


// err_buffer must be a 128 byte string
int    get_packet_error(ping_packet_t pk, char *err_buffer)
{
    switch (pk.icmp.type)
    {
        case ICMP_ECHOREPLY: // echo reply
            return (0);

        case ICMP_DEST_UNREACH: // destination unreachable
            switch (pk.icmp.code)
            {
                case ICMP_NET_UNREACH: // destination network unreachable
                    strcpy(err_buffer, "destination unreachable: destination network unreachable");
                    break;
                case ICMP_HOST_UNREACH: // destination host unreachable
                    strcpy(err_buffer, "destination unreachable: destination host unreachable");
                    break;
                case ICMP_PROT_UNREACH: // destination protocol unreachable
                    strcpy(err_buffer, "destination unreachable: destination protocol unreachable");
                    break;
                case ICMP_PORT_UNREACH: // destination port unreachable
                    strcpy(err_buffer, "destination unreachable: destination port unreachable");
                    break;
                case ICMP_FRAG_NEEDED: // fragmentation required and df set
                    strcpy(err_buffer, "destination unreachable: fragmentation required and df set");
                    break;
                case ICMP_SR_FAILED: // source route failed
                    strcpy(err_buffer, "destination unreachable: source route failed");
                    break;
                case ICMP_NET_UNKNOWN: // destination network unknown
                    strcpy(err_buffer, "destination unreachable: destination network unknown");
                    break;
                case ICMP_HOST_UNKNOWN: // destination host unknown
                    strcpy(err_buffer, "destination unreachable: destination host unknown");
                    break;
                case ICMP_HOST_ISOLATED: // source host isolated
                    strcpy(err_buffer, "destination unreachable: source host isolated");
                    break;
                case ICMP_NET_ANO: // Network administratively prohibited
                    strcpy(err_buffer, "destination unreachable: Network administratively prohibited");
                    break;
                case ICMP_HOST_ANO: // Host administratively prohibited
                    strcpy(err_buffer, "destination unreachable: Host administratively prohibited");
                    break;
                case ICMP_NET_UNR_TOS: // Network unreachable for Type Of Service
                    strcpy(err_buffer, "destination unreachable: Network unreachable for Type Of Service");
                    break;
                case ICMP_HOST_UNR_TOS: // Host unreachable for Type of Service
                    strcpy(err_buffer, "destination unreachable: Host unreachable for Type Of Service");
                    break;
                case ICMP_PKT_FILTERED: // Administratively prohibited
                    strcpy(err_buffer, "destination unreachable: Administratively prohibited");
                    break;
                case ICMP_PREC_VIOLATION: // precedence violation
                    strcpy(err_buffer, "destination unreachable: precedence violation");
                    break;
                case ICMP_PREC_CUTOFF: // precedence cut off
                    strcpy(err_buffer, "destination unreachable: precedence cut off");
                    break;
                default:
                    strcpy(err_buffer, "destination unreachable: unknown error");
                    break;
            }
            return (ICMP_DEST_UNREACH);
        
        case ICMP_SOURCE_QUENCH: // source quench | traffic congestion controll
            return (ICMP_SOURCE_QUENCH);
        
        case ICMP_REDIRECT: // redirect message
            switch (pk.icmp.code)
            {
                case 0: // 	Redirect Datagram for the Network
                    break;
                case 1: //	Redirect Datagram for the Host
                    break;
                case 2: //	Redirect Datagram for the Type of Service & network
                    break;
                case 3: //	Redirect Datagram for the Type of Service & host
                    break;
                default:
                    break;
            }
            return (ICMP_REDIRECT);
        
        case 6: // Alternate host address
            return (6);
        
        case ICMP_ECHO: // echo request
            return (0);
        
        case 9: // router advertisement
            break;
    
        case 10: // router solicitation
            break;
        
        case ICMP_TIME_EXCEEDED: // time exceeded
            switch (pk.icmp.code)
            {
                case 0: // TTL exceeded
                    break;
                case 1: // Fragment reassembly time exceeded
                    break;
                default:
                    break;
            }
            return (ICMP_TIME_EXCEEDED);
        
        case ICMP_PARAMETERPROB: // parameter problem
            switch (pk.icmp.code)
            {
                case 0: // Pointer indicates the error
                    strcpy(err_buffer, "parameter problem: pointer indicates the error");
                    break;
                case 1: // Missing a required option
                    strcpy(err_buffer, "parameter problem: missing a required option");
                    break;
                case 2: // Bad length
                    strcpy(err_buffer, "parameter problem: bad length");
                    break;
                default:
                    strcpy(err_buffer, "parameter problem: unknown error");
                    break;
            }
            return (ICMP_PARAMETERPROB);
        
        case ICMP_TIMESTAMP: // timestamp request
            break;
        
        case ICMP_TIMESTAMPREPLY: // timestamp reply
            break;
        
        case ICMP_INFO_REQUEST: // information request
            break;
        
        case ICMP_INFO_REPLY: // information_reply
            break;
    
        case ICMP_ADDRESS: // address mask request
            break;
        
        case ICMP_ADDRESSREPLY: // address mask reply
            break;
        
        case 30: // information request (traceroute)
            break;

        case 31: // datagram conversion error
            break;
        
        case 32: // mobile host redirect
            break;
        
        case 33: // where are you
            break;

        case 34: // here i am
            break;

        case 35: // mobile registration request
            break;
        
        case 36: // mobile registration reply
            break;
        
        case 37: // DNS request
            break;
        
        case 38: // DNS reply
            break;
        
        default: // reserved
            break;
    }
    printf("ICMP get_error: type out of bounds: %d (code: %d)\n", pk.icmp.type, pk.icmp.code);
    return (1);
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
    
    printf ("PING %s (%s): %lu data bytes\n", ping_hostname, ctx.stats.host_addr, sizeof(ping_packet_t) - (sizeof(struct iphdr) + sizeof(struct icmphdr)));

    // calling alarm handler for no-delay 1st packet send
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

        // add time difference data
        time_t time_difference = get_difference_time(received_packet.time, now);
        ctx_add_package_stat(time_difference);

        // check for errors in packet
        char err_message[128];
        bzero(err_message, 128);
        int err = get_packet_error(received_packet, (char*)&err_message);
        if (err != 0)
        {
            printf ("error occured: %s\n", err_message);
            alarm(1);
            continue ;
        }


        // TODO retrieve from received_message
        char *resolved_hostname = resolve_address_from_int(ctx.sockaddr->sin_family, received_packet.ip.saddr);//ctx.sockaddr->sin_family, );
    
        // Print ping infos
        printf ("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.3fms\n",
            received_bytes,
            resolved_hostname,
            received_packet.icmp.un.echo.sequence, 
            received_packet.ip.ttl,
            (float)(time_difference) / 1000.0f);
        
        free (resolved_hostname);

        // sending next ping through alarm handler
        alarm(1);
    }
}
