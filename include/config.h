#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

typedef struct 
{
    int mode;            // 0 = none, 1 = ping, 2 = traceroute
    char host[256];

    // Ping options
    uint32_t count;
    uint32_t size;
    uint32_t timeout_ms;
    uint8_t ttl;

} config_t;

void configure_firewall(int enable);

#endif // CONFIG_H