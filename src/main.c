#include <stdio.h>
#include "ping.h"
#include "trace.h"
#include "config.h"
#include "argparser.h"

int main(int argc, char *argv[])
{
    if (initialize_winsock() != 0) 
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    configure_firewall(1);

    config_t cfg;
    if (parse_arguments(argc, argv, &cfg) != 0) 
    {
        printf("Usage: pingtrace -p|-t <host> [options]\n");
        cleanup_socket_library();
        return 1;
    }

    if (cfg.mode == 1)   // ping
    {
        perform_ping(
            cfg.host,
            cfg.count,
            cfg.size,
            cfg.timeout_ms,
            cfg.ttl
        );
    }
    else if (cfg.mode == 2) // traceroute
    {
        perform_traceroute(cfg.host, 30, 5000);
    }

    configure_firewall(0);

    cleanup_socket_library();
    return 0;
}
