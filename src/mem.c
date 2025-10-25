#include <stdio.h>
#include <string.h>
#include "mem.h"

int get_mem_info(MemInfo *info){
  char key[KEY_SIZE];
  unsigned long value;
  char unit[UNIT_SIZE];
  unsigned long mem_total, mem_available;

  FILE *fp = fopen("/proc/meminfo","r");
  if (!fp) return -1;

  mem_total = 0;
  mem_available = 0;
  while(fscanf(fp, "%63s %lu %15s\n", key, &value, unit) == 3) {
    if (strcmp(key, "MemTotal:") == 0) mem_total = value;
    if (strcmp(key, "MemAvailable:") == 0) mem_available = value;
    if (mem_total && mem_available) break;
  }
  fclose(fp);

  info->total_kb = mem_total;
  info->used_kb = mem_total - mem_available;

  return 0;
}
