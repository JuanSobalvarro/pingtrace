# 1. Ping – Detailed Component Architecture
┌──────────────────────────┐
│     CLI / Output Layer   │
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│     Ping Controller       │  ← main logic
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│   ICMP Packet Engine      │  ← build/parse ICMP packets
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│      Raw Socket Layer     │  ← send/receive packets
└───────────────────────────┘

## Sequence Diagram
User → CLI: ping 8.8.8.8
CLI → Controller: initialize
Controller → Resolver: resolve DNS
Controller → ICMP Engine: build_echo_request
Controller → Socket: send(request)
Socket → Controller: receive(reply)
Controller → ICMP Engine: parse(reply)
Controller → CLI: print RTT

## A. CLI / Output Layer

- Parses arguments: target, count, timeout, interval
- Displays results:
  - RTT per packet
  - Lost packets
  - min/avg/max/stddev

This layer never touches sockets.

## B. Ping Controller
- Controls the logic of a ping session.
- Responsibilities:
  - Resolve hostname → IP
  - Loop over each packet:
  - build ICMP Echo Request
  - send packet
  - wait for Echo Reply
  - measure RTT
  - store stats

- Stop on:
  - count reached
  - keyboard interrupt

## C. ICMP Packet Engine

- Handles all ICMP-level operations.
- Functions:

- Build ICMP Echo Request:

type: 8

code: 0

id: process ID

seq: incremental

payload: timestamp / pattern data

checksum calculation

Parse Echo Reply:

type: 0

extract id, seq, payload

validate checksum

D. Raw Socket Layer

Low-level socket functions:

Create raw socket

Set TTL (optionally)

Set timeout

Send packets to target

Receive packets from network buffer

Extract the ICMP portion from the IP packet

This layer is OS-dependent.

# traceroute Architecture
┌──────────────────────────┐
│     CLI / Output Layer   │
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│  Traceroute Controller    │  ← loop over TTLs
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│   ICMP Packet Engine      │  ← reuse Ping’s packet logic
└───────────────┬──────────┘
                │
┌───────────────▼──────────┐
│ Raw Socket Layer (TTL API)│
└───────────────────────────┘
# 2. Traceroute – Detailed Component Architecture

A. CLI / Output Layer

Arguments: max hops, probes per hop, timeout

Displays per-hop results:

hostname

IP

RTTs (per probe)

"*" for timeout

B. Traceroute Controller

Main loop:

Resolve hostname → IP

For ttl = 1 to max_hops:

For each probe:

Build ICMP Echo Request

Send with TTL = current value

Wait for response:

If ICMP Time Exceeded → intermediate hop

If ICMP Echo Reply → destination reached

Print hop info

Stop when:

Destination reached

Reached max hops

Traceroute always relies on routers decrementing TTL and sending ICMP Time Exceeded.

C. ICMP Packet Engine

Same as Ping: build/send/parse ICMP Echo Request.
Traceroute does not generate its own Time Exceeded packets; it only receives them.

For replies, traceroute must detect:

ICMP type 11 → Time Exceeded

ICMP type 0 → Echo Reply

Additionally, traceroute often needs to match:

sequence number

echo request identification

D. Raw Socket Layer with TTL

Must support:

setsockopt(IP_TTL) before sending each packet

send and receive raw ICMP packets

detect the sender IP

apply timeouts correctly (per hop, and per probe)

Traceroute – Sequence Diagram
User → CLI: traceroute google.com
CLI → Controller: initialize
Controller → Resolver: resolve DNS
loop TTL=1..30:
    Controller → ICMP Engine: build_echo_request
    Controller → Socket: send(request, TTL)
    Socket → Controller: receive(any_reply)
    Controller: check type:
        - type 11 → intermediate node
        - type 0 → destination
    Controller → CLI: print hop result