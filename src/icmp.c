#include "icmp.h"
#include "helpers.h"
#include "socket.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h> 
#include <string.h> // For strlen

typedef byte uint8_t;

int send_icmp_request(SOCKET sock, const char *ip_str, icmp_echo_t *request, uint32_t packet_size)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    inet_pton(AF_INET, ip_str, &(dest_addr.sin_addr));

    // printf("inet pton is safe\n");

    int bytes_sent = sendto(sock, (const char *)request, packet_size, 0,
                            (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    // printf("Send to is safe\n");

    if (bytes_sent == SOCKET_ERROR) 
    {
        perror("Failed to send ICMP request");
        return -1;
    } 
    else 
    {
        // printf("Sent %d bytes to %s\n", bytes_sent, ip_str);
        return bytes_sent;
    }
}

int receive_icmp_reply(
    SOCKET sock,
    char *recv_buffer,
    int buffer_len,
    struct sockaddr_in *sender_addr,
    uint32_t timeout_ms)
{
    int addr_len = sizeof(*sender_addr);

    // WINDOWS: timeouts are DWORD in milliseconds (not timeval!)
    DWORD tv = timeout_ms;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                   (const char*)&tv, sizeof(tv)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to set socket timeout. Error: %d\n", WSAGetLastError());
        return -1;
    }

    // Receive raw IP packet
    int bytes_received = recvfrom(
        sock,
        recv_buffer,
        buffer_len,
        0,
        (struct sockaddr *)sender_addr,
        &addr_len
    );

    if (bytes_received == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if (err != WSAETIMEDOUT)
            fprintf(stderr, "Failed to receive ICMP reply. Error: %d\n", err);
        return -1;
    }

    // Sanity: must have at least IP header
    if (bytes_received < sizeof(struct ip_header))
    {
        // fprintf(stderr, "Received packet too small for IP header: %d bytes\n", bytes_received);
        return -1;
    }

    // Extract IP header of response
    struct ip_header *ip_hdr = (struct ip_header *)recv_buffer;
    int ip_header_len = (ip_hdr->ihl_version & 0x0F) * 4;

    // Basic sanity check
    // if (ip_header_len < 20 || ip_header_len > 60)
    //     return -1;

    // if (bytes_received < ip_header_len + sizeof(icmp_header_t))
    //     return -1;

    // all goodd
    return bytes_received;
}


icmp_echo_t* create_icmp_request(uint32_t data_size)
{
    // printf("Creating icmp request\n");
    icmp_echo_t* request;
    size_t packet_size = sizeof(icmp_echo_t) + data_size;


    request = (icmp_echo_t*) malloc(packet_size); 
    // printf("Allocated memory %d bytes\n", packet_size);
    if (request == NULL) 
    {
        perror("Failed to allocate memory for ICMP request");
        return NULL;
    }

    request->header.type = 8; 
    request->header.code = 0;
    request->header.checksum = 0; 
    request->identifier = (uint16_t) GetCurrentProcessId();
    request->sequence_number = 0;

    // always the request header is sizeof(icmp_echo_request_t) which for icmp echo request is 8 bytes
    // the data follows directly after the struct in memory

    // printf("Headers ready\n");

    const char* data = "uwunyanichan"; 
    size_t data_len = strlen(data);

    for (size_t i = 0; i < data_size; i++) 
    {
        // use pointer arithmetic to fill data
        ((char*)request)[sizeof(icmp_echo_t) + i] = data[i % data_len];
        // request = data[i % data_len];
    }

    // printf("Data filled\n");

    request->header.checksum = calculate_checksum((uint16_t *)request, (uint16_t) packet_size);

    // printf("request size %zu\n", sizeof(icmp_echo_request_t));

    return request;
}