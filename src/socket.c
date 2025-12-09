#include "socket.h"
#include <stdio.h>

int initialize_winsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) 
    {
        perror("WSAStartup failed");
        return -1;
    }
    return 0;
}

void cleanup_socket_library() 
{
    WSACleanup();
    printf("Winsock cleaned up.\n");
}

SOCKET create_icmp_socket()
{
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sock == INVALID_SOCKET) 
    {
        perror("Failed to create socket");
        return INVALID_SOCKET;
    }

    // Bind to INADDR_ANY to receive replies
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = 0;

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) 
    {
        perror("Failed to bind socket");
        closesocket(sock);
        return INVALID_SOCKET;
    }

    return sock;
}

void close_socket(SOCKET sock)
{
    closesocket(sock);
}