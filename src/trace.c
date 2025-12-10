#include "trace.h"
#include "icmp.h"
#include "socket.h"
#include "helpers.h"
#include <windows.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <stdio.h>

void perform_traceroute(const char *host, uint32_t max_hops, uint32_t timeout_ms)
{
    // Resolve host to IP
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    struct addrinfo *result = NULL;
    if (getaddrinfo(host, NULL, &hints, &result) != 0) 
    {
        printf("Traceroute request could not find host %s. Please check the name and try again.\n", host);
        return;
    }
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in*)result->ai_addr)->sin_addr, ip_str, sizeof(ip_str));
    freeaddrinfo(result);

    printf("Tracing route to %s [%s]\n", host, ip_str);
    printf("Over a maximum of %u hops:\n\n", max_hops);

    init_high_res_timer();

    SOCKET sock = create_icmp_socket();
    if (sock == INVALID_SOCKET) 
    {
        printf("Failed to create ICMP socket.\n");
        return;
    }

    uint16_t seq = 0;

    for (uint32_t hop = 1; hop <= max_hops; hop++) 
    {
        int ttl = hop;
        setsockopt(sock, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));

        printf("%3u  ", hop);

        int received_any = 0;
        char last_ip[INET_ADDRSTRLEN] = "";

        for (int probe = 0; probe < 3; probe++)
        {
            // Build ICMP packet
            icmp_echo_t *req = create_icmp_request(32);
            size_t packet_size = sizeof(icmp_echo_t) + 32;
            req->sequence_number = seq++;
            req->header.checksum = 0;
            req->header.checksum = calculate_checksum((uint16_t*)req, (uint16_t)packet_size);

            uint64_t send_time = get_performance_count();
            send_icmp_request(sock, ip_str, req, (uint32_t)packet_size);
            free(req);

            // Receive
            char buffer[MAX_PACKET_SIZE];
            struct sockaddr_in sender;
            int r = receive_icmp_reply(sock, buffer, sizeof(buffer), &sender, timeout_ms);

            if (r < 0)
            {
                printf("* ");
                continue;
            }
            
            received_any = 1;
            uint64_t end_time = get_performance_count();
            uint64_t rtt = counts_to_ms(send_time, end_time);

            inet_ntop(AF_INET, &sender.sin_addr, last_ip, sizeof(last_ip));

            printf("%llums ", rtt);
        }

        if (received_any)
            printf("%s\n", last_ip);
        else
            printf("\n");

        // Stop when destination replied
        if (strcmp(last_ip, ip_str) == 0)
        {
            printf("\nTrace complete.\n");
            break;
        }

        Sleep(200);
    }

    printf("\nTrace complete. Maximum hops reached.\n");
    close_socket(sock);
}