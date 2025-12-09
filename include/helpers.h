#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <windows.h>

uint16_t calculate_checksum(uint16_t *buf, int len);

void init_high_res_timer(); 
uint64_t get_performance_count(); 
uint64_t counts_to_ms(uint64_t start_count, uint64_t end_count);

#endif // HELPERS_H