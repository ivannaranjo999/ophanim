#ifndef BAT_H
#define BAT_H

#define STATUS_SIZE 32

typedef struct{
  float capacity;
  char status[STATUS_SIZE];
  int present;
} battery_info_t;

battery_info_t read_battery_info(const char *sys_path);

#endif
