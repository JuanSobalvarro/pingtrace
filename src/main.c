/*
* Main entry point for pingtrace, based on arguments we will run a ping (-p)
* or a traceroute (-t) to a given host.
*/
#include <stdio.h>
#include <string.h>
#include "ping.h"
#include "trace.h"
#include "socket.h"
#include "icmp.h"

int main(int argc, char *argv[])
{
    if (initialize_winsock() != 0) 
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    if (argc < 3) 
    {
        printf("Usage: %s -p|-t <host> [other options]\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "-p") == 0) 
    {
        perform_ping(argv[2], 4, 32, 1000);  // Default: 4 pings, 32 bytes, 1000ms timeout
    } 
    else if (strcmp(argv[1], "-t") == 0) 
    {
        perform_traceroute(argv[2], 30, 5000);  // Default: 30 max hops, 5000ms timeout
    } 
    else 
    {
        printf("Invalid option: %s\n", argv[1]);
        printf("Usage: %s -p|-t <host> [other options]\n", argv[0]);
        return 1;
    }

    cleanup_socket_library();

    return 0;
}