#ifndef MEM_H
#define MEM_H


#define KEY_SIZE 64
#define UNIT_SIZE 16

typedef struct {
  unsigned long long total_kb;
  unsigned long long used_kb;
} MemInfo;

int get_mem_info (MemInfo *info);

#endif
