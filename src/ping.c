#include "ping.h"

void perform_ping(const char *host, uint32_t count, uint32_t data_size, uint32_t timeout_ms)
{
    // Resolve host to IP
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    struct addrinfo *result = NULL;
    if (getaddrinfo(host, NULL, &hints, &result) != 0) {
        printf("Ping request could not find host %s. Please check the name and try again.\n", host);
        return;
    }
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((struct sockaddr_in*)result->ai_addr)->sin_addr, ip_str, sizeof(ip_str));
    freeaddrinfo(result);

    printf("Pinging %s [%s] with %u bytes of data:\n", host, ip_str, data_size);

    init_high_res_timer();

    SOCKET sock = create_icmp_socket();
    if (sock == INVALID_SOCKET) 
    {
        printf("Failed to create ICMP socket. Invalid socket.\n");
        return;
    }

    uint32_t sent = 0, received = 0;
    uint64_t min_time = UINT64_MAX, max_time = 0, total_time = 0;
    uint16_t seq = 0;

    for (uint32_t i = 0; i < count; i++) 
    {
        icmp_echo_t *request = create_icmp_request(data_size);
        size_t packet_size = data_size + sizeof(icmp_echo_t);
        if (request == NULL) 
        {
            printf("Failed to create ICMP request. Maybe memory?\n");
            continue;
        }
        request->sequence_number = seq++;
        request->header.checksum = 0;
        request->header.checksum = calculate_checksum((uint16_t*)request, (uint16_t) packet_size);
        
        int bytes_sent = send_icmp_request(sock, ip_str, request, (uint32_t) packet_size);
        uint64_t send_time = get_performance_count();
        
        if (bytes_sent < 0) 
        {
            free(request);
            printf("Failed to send ICMP request.\n");
            continue;
        }
        
        sent++;

        size_t bytes_received = 0;
        struct sockaddr_in sender_addr;
        char recv_buffer[MAX_PACKET_SIZE];
        
        // Receive reply
        icmp_echo_t* echo_reply = receive_ping_reply(sock, request->sequence_number, timeout_ms, recv_buffer, &sender_addr, &bytes_received);
        uint64_t end_time = get_performance_count();

        if (echo_reply == NULL) 
        {
            free(request);
            continue;
        }


        uint64_t rtt = counts_to_ms(send_time, end_time);

        received++;
        total_time += rtt;
        
        if (rtt < min_time) min_time = rtt;
        if (rtt > max_time) max_time = rtt;

        char sender_ip[INET_ADDRSTRLEN];

        struct ip_header *ip_hdr = (struct ip_header*)recv_buffer;

        inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, sizeof(sender_ip));
        printf("Reply from %s: bytes=%zu time=%llums TTL=%u\n", sender_ip, bytes_received, rtt, ip_hdr->ttl);
        
        free(request);

        // Wait a bit between pings
        Sleep(1000); // 1 second
    }

    close_socket(sock);

    printf("\nPing statistics for %s:\n", host);
    printf("    Packets: Sent = %u, Received = %u, Lost = %u (%u%% loss),\n", sent, received, sent - received, sent > 0 ? (sent - received) * 100 / sent : 0);
    
    if (received > 0) 
    {
        printf("Approximate round trip times in milli-seconds:\n");
        printf("    Minimum = %llums, Maximum = %llums, Average = %llums\n", min_time, max_time, total_time / received);
    }
}

icmp_echo_t* receive_ping_reply(SOCKET sock, size_t expected_seq, uint32_t timeout_ms, char* recv_buffer, struct sockaddr_in* sender_addr, size_t* bytes_received_out)
{
    int bytes_received = receive_icmp_reply(sock, recv_buffer, MAX_PACKET_SIZE, sender_addr, timeout_ms);
    
    if (bytes_received < 0) 
    {
        printf("Request timed out.\n");
        return NULL;
    }

    // Parse IP header
    struct ip_header *ip_hdr = (struct ip_header*)recv_buffer;
    int ip_header_len = (ip_hdr->ihl_version & 0x0F) * 4;
    if (bytes_received < ip_header_len + sizeof(icmp_header_t)) 
    {
        printf("Received packet too small.\n");
        return NULL;
    }

    // Parse ICMP header
    icmp_header_t *icmp_hdr = (icmp_header_t*)(recv_buffer + ip_header_len);
    if (icmp_hdr->type != 0 || icmp_hdr->code != 0) 
    {
        printf("Received non-echo reply ICMP packet.\n");
        return NULL;
    }

    icmp_echo_t *echo_reply = (icmp_echo_t*)icmp_hdr;
    if (echo_reply->identifier != (uint16_t)GetCurrentProcessId() || echo_reply->sequence_number != expected_seq) 
    {
        printf("Received reply with mismatched identifier or sequence.\n");
        return NULL;
    }

    *bytes_received_out = bytes_received - ip_header_len - sizeof(icmp_echo_t);

    return echo_reply;
}
