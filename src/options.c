#include "options.h"


int get_options(int ac, char **av)
{
    int i;
    int j;
    int options;

    i = 1;
    options = 0;
    while (i < ac)
    {
        if (*av[i] == '-')
        {
            j = 1;
            while (av[i][j])
            {
                switch (av[i][j])
                {
                    case 'v':
                        options |= OPT_VERBOSE;
                        break;
                    case '?':
                        options |= OPT_USAGE;
                        break;
                    default:
                        dprintf(2, "%s: invalid option -- '%c'\n", av[0], av[i][j]);
                        dprintf(2, "Try '%s -?' for more informations\n", av[0]);
                        return (-1);
                }
                ++j;
            }
        }
        ++i;
    }
    return (options);
}



int print_usage(char *pname)
{
    printf("Usage: %s [OPTION...] HOST\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
\n\
 Options valid for all request types:\n\
  -v                         verbose output\n\
\n\
 Options valid for --echo requests:\n\
  -?                         give this help list\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\n\
for any corresponding short options.\n\
\n\
Options marked with (root only) are available only to superuser.\n", pname);
    return (0);
}




int get_host_arg(int ac, char **av)
{
    int i;

    i = 1;
    while (i < ac)
    {
        if (*av[i] != '-')
            return (i);
        ++i;
    }
    return (-1);
}

