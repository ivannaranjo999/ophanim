#ifndef NET_H
#define NET_H

#define NET_LINE_SIZE 512
#define NET_NAME_SIZE 64

typedef struct {
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
} NetStats;

int get_net_stats(NetStats *stats, const char *iface);

#endif
