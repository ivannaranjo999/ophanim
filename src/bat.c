#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bat.h"

battery_info_t read_battery_info(const char *sys_path) {
  battery_info_t bat;
  bat.capacity = 0.0f;
  strcpy(bat.status, "Unknown");
  bat.present = 0;

  char path[256];
  FILE *fp;

  snprintf(path, sizeof(path), "%s/class/power_supply/BAT0", sys_path);
  if (access(path, F_OK) != 0) {
    snprintf(path, sizeof(path), "%s/class/power_supply/BAT1", sys_path);
    if (access(path, F_OK) != 0) {
      return bat;
    }
  }
  bat.present = 1;

  snprintf(path, sizeof(path), "%s/class/power_supply/BAT0/capacity", sys_path);
  fp = fopen(path, "r");
  if (fp) {
    int cap;
    if (fscanf(fp, "%d", &cap) == 1) {
      bat.capacity = (float)cap;
    }
    fclose(fp);
  } else {
    char path_now[256], path_full[256];
    snprintf(path_now, sizeof(path_now), "%s/class/power_supply/BAT0/energy_now", sys_path);
    snprintf(path_full, sizeof(path_full), "%s/class/power_supply/BAT0/energy_full", sys_path);
    FILE *f_now = fopen(path_now, "r");
    FILE *f_full = fopen(path_full, "r");
    if (f_now && f_full) {
      long long now, full;
      if (fscanf(f_now, "%lld", &now) == 1 && fscanf(f_full, "%lld", &full) == 1 && full > 0) {
        bat.capacity = (float)now / (float)full * 100.0f;
      }
      fclose(f_now);
      fclose(f_full);
    }
  }

  snprintf(path, sizeof(path), "%s/class/power_supply/BAT0/status", sys_path);
  fp = fopen(path, "r");
  if (fp) {
    if (fgets(bat.status, sizeof(bat.status), fp)) {
    bat.status[strcspn(bat.status, "\n")] = '\0';
  }
    fclose(fp);
  }

  return bat;
}

