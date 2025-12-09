#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include <winsock2.h>

#define MAX_PACKET_SIZE 1024

struct icmp_header 
{
    uint8_t type; // first byte
    uint8_t code; // second byte
    uint16_t checksum; // third and fourth bytes
} typedef icmp_header_t;

struct icmp_echo 
{
    icmp_header_t header;
    uint16_t identifier;
    uint16_t sequence_number;
    // char* data; data shouldnt be a pointer, should be added directly after the struct in memory
} typedef icmp_echo_t;


icmp_echo_t* create_icmp_request(uint32_t size);

int send_icmp_request(SOCKET sock, const char *ip_str, icmp_echo_t *request, uint32_t packet_size);

int receive_icmp_reply(SOCKET sock, char *recv_buffer, int buffer_len, struct sockaddr_in *sender_addr, uint32_t timeout_ms);

#endif // ICMP_H