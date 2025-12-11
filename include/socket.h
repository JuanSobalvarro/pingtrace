#ifndef SOCKET_H
#define SOCKET_H

#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

struct ip_header {
    uint8_t ihl_version; // 4-bit version, 4-bit header length (IHL)
    uint8_t tos;         // Type of Service
    uint16_t total_length;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t ttl;         // Time to Live
    uint8_t protocol;    // Protocol (1 for ICMP)
    uint16_t checksum;
    struct in_addr src_addr;
    struct in_addr dest_addr;
};

int initialize_winsock();

void cleanup_socket_library();

SOCKET create_icmp_socket(uint8_t ttl);

void close_socket(SOCKET sock);

#endif // SOCKET_H