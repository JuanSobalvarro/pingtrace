#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "icmp.h"
#include "socket.h"
#include "helpers.h"

void perform_ping(const char *host, uint32_t count, uint32_t size, uint32_t timeout_ms, uint8_t ttl);

icmp_echo_t* receive_ping_reply(SOCKET sock, size_t expected_seq, uint32_t timeout_ms, char* recv_buffer, struct sockaddr_in* sender_addr, size_t* bytes_received_out);

#endif // PING_H