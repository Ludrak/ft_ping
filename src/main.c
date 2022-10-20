
#include "ping.h"

typedef uint8_t bool_t;

ping_ctx_t ctx;


void on_interrupt(int sig)
{
    printf("Interrupted. (%d)\n", sig);
    exit (sig);
}

void construct_ping_packet()
{
    struct iphdr   ip_header = construct_ping_iphdr(ctx.sockaddr);
    struct icmphdr icmp_header = construct_ping_icmphdr();

    printf ("constructed icmp header:\n");
    print_icmphdr(icmp_header);
    printf ("constructed ip header:\n");
    print_iphdr(ip_header);

    construct_packet(ctx.packet, ip_header, icmp_header);
}

void on_alarm(int sig)
{
    (void)sig;
    construct_ping_packet();
    ssize_t bytes = sendto(ctx.socket, &ctx.packet, sizeof(ctx.packet), 0, (struct sockaddr*)&ctx.sockaddr, sizeof(ctx.sockaddr));
    if (bytes < 0)
    {
        print_failed("sendto()", errno);
        return ;
    }
    printf ("Send of %zd bytes successfull\n", bytes);

}



int main(int ac, char **av)
{
    if (ac < 2)
    {
        fprintf (stderr, "not enought parameters, expected: ft_ping <address>\n");
        return (1);
    }
    printf ("running ping with user %d\n", getuid());

    string_address_t ping_address = av[1];

    // setting up signals handlers
    signal(SIGALRM, on_alarm);
    signal(SIGINT, on_interrupt);

    // initialize ctx
    int init_err = init_ctx();
    if (init_err != 0)
        return (init_err);
    
    // setting up sockaddr
    init_ctx_addr(AF_INET, ping_address);
    print_sockaddr_in(ctx.sockaddr);

    // setting up icmp header packet
   // construct_ping_iphdr();
   // construct_ping_icmphdr();
   // print_icmphdr(ctx.icmp_header);

    

    // sending ping through alarm handler
    alarm(1);
   


    // recieve loop
    while (1)
    {
        struct msghdr  recv_packet;
        bzero(&recv_packet, sizeof(recv_packet));

        ssize_t recieved_bytes = recvmsg(ctx.socket, &recv_packet, MSG_CONFIRM); 
        if (recieved_bytes < 0)
        {
            print_failed("recvmsg()", errno);
            return 1;
        }
        printf ("Received %zd bytes\n", recieved_bytes);
        print_msghdr(recv_packet);

        // sending next ping through alarm handler
        //alarm(1);
    }
}
