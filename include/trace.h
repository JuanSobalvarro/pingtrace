#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>

void perform_traceroute(const char *host, uint32_t max_hops, uint32_t timeout_ms);

#endif // TRACE_H