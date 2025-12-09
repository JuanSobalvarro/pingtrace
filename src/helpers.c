#include "helpers.h"
#include <stdio.h> 

// Global variables for the high-resolution timer
static LARGE_INTEGER g_frequency;
static int g_timer_initialized = 0;

uint16_t calculate_checksum(uint16_t *buf, int len)
{
    uint32_t sum = 0;

    for (; len > 1; len -= 2) 
    {
        sum += *buf++;
    }

    if (len == 1) 
    {
        sum += *(uint8_t *)buf;
    }
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return (uint16_t)(~sum);
}

void init_high_res_timer() 
{
    if (QueryPerformanceFrequency(&g_frequency))
    {
        g_timer_initialized = 1;
    }
    else
    {
        g_timer_initialized = 0;
        fprintf(stderr, "Error: QueryPerformanceFrequency failed.\n");
    }
}

uint64_t get_performance_count() 
{
    LARGE_INTEGER current_count;
    if (QueryPerformanceCounter(&current_count))
    {
        return current_count.QuadPart;
    }
    else
    {
        return 0; 
    }
}

uint64_t counts_to_ms(uint64_t start_count, uint64_t end_count)
{
    if (!g_timer_initialized || g_frequency.QuadPart == 0)
    {
        return 0;
    }

    uint64_t elapsed_counts = (end_count - start_count);

    // printf("Elapsed counts: %llu, Frequency: %llu\n", elapsed_counts, g_frequency.QuadPart);

    return elapsed_counts * 1000 / g_frequency.QuadPart;
}