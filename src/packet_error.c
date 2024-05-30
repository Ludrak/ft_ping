#include "packet.h"



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
        {
            strcpy(err_buffer, "source quench");
            return (ICMP_SOURCE_QUENCH);
        }
        case ICMP_REDIRECT: // redirect message
            switch (pk.icmp.code)
            {
                case 0: // 	Redirect Datagram for the Network
                    strcpy(err_buffer, "redirected: redirected datagram for the network");
                    break;
                case 1: //	Redirect Datagram for the Host
                    strcpy(err_buffer, "redirected: redirected datagram for the host");
                    break;
                case 2: //	Redirect Datagram for the Type of Service & network
                    strcpy(err_buffer, "redirected: redirected datagram for the type of service and network");
                    break;
                case 3: //	Redirect Datagram for the Type of Service & host
                    strcpy(err_buffer, "redirected: redirected datagram for the type of service and host");
                    break;
                default:
                    break;
            }
            return (ICMP_REDIRECT);
        
        case 6: // Alternate host address
            strcpy(err_buffer, "alternate host address");
            return (6);
        
        case ICMP_ECHO: // echo request
            return (0);
        
        case 9: // router advertisement
            strcpy(err_buffer, "router advertisement");
            break;
    
        case 10: // router solicitation
            strcpy(err_buffer, "router solicitation");
            break;
        
        case ICMP_TIME_EXCEEDED: // time exceeded
            switch (pk.icmp.code)
            {
                case 0: // TTL exceeded
                    strcpy(err_buffer, "time exceeded: time to live exceeded");
                    break;
                case 1: // Fragment reassembly time exceeded
                    strcpy(err_buffer, "time exceeded: fragment reassembly time exceeded");
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
            strcpy(err_buffer, "timestamp request");
            break;
        
        case ICMP_TIMESTAMPREPLY: // timestamp reply
            strcpy(err_buffer, "timestamp reply");
            break;
        
        case ICMP_INFO_REQUEST: // information request
            strcpy(err_buffer, "information request");
            break;
        
        case ICMP_INFO_REPLY: // information_reply
            strcpy(err_buffer, "information reply");
            break;
    
        case ICMP_ADDRESS: // address mask request
            strcpy(err_buffer, "address mask request");
            break;
        
        case ICMP_ADDRESSREPLY: // address mask reply
            strcpy(err_buffer, "address mask reply");
            break;
        
        case 30: // information request (traceroute)
            strcpy(err_buffer, "information request (traceroute)");
            break;

        case 31: // datagram conversion error
            strcpy(err_buffer, "datagram conversion error");
            break;
        
        case 32: // mobile host redirect
            strcpy(err_buffer, "mobile host redirect");
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

