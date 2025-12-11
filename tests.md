# Reachable hosts and low TTL tests
- 8.8.8.8 (google dns)
- 1.1.1.1 (cloudflare dns)

# Destination unreachable (host)
- 10.255.255.1 (non-routable IP in private range)

# Destination unreachable (port) here we need filtering of ICMP messages
- 8.8.8.8

# Network unreachable
- 203.0.113.0/24 (non-routable TEST-NET-3 range)