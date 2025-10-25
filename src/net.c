#include <stdio.h>
#include <string.h>
#include "net.h"

extern const char *proc_path;

int get_net_stats(NetStats *stats, const char *iface) {
  char line[NET_LINE_SIZE];
  char name[NET_NAME_SIZE];
  int found = 0;
  unsigned long long r_bytes, t_bytes;
  char path[NET_LINE_SIZE];

  snprintf(path, sizeof(path), "%s/net/dev", proc_path);
  FILE *fp = fopen(path, "r");
  if (!fp) return -1;

  while (fgets(line, sizeof(line), fp)) {
    if (sscanf(line, " %63[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu",
      name, &r_bytes, &t_bytes) == 3) {
      if (strcmp(name, iface) == 0) {
          stats->rx_bytes = r_bytes;
          stats->tx_bytes = t_bytes;
          found = 1;
          break;
      }
    }
  }
  fclose(fp);
  return found ? 0 : -1;
}

