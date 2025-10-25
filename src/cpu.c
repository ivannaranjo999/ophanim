#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpu.h"

extern const char *proc_path;

static int read_cpu_times(CpuTimes *t) {
  char line[CPU_LINE_SIZE];
  char path[CPU_LINE_SIZE];

  /* Open proc stat file */
  snprintf(path,sizeof(path), "%s/stat", proc_path);
  FILE *fp = fopen(path, "r");
  if (!fp) return -1;

  /* Read the first line */
  if (!fgets(line, sizeof(line),fp)) {
    fclose(fp);
    return -1;
  }
  fclose (fp);

  if(!sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
    &t->user,&t->nice,&t->system,&t->idle,
    &t->iowait,&t->irq,&t->softirq,&t->steal)){
    return -1;
  }

  return 0;
}

double get_cpu_usage() {
  CpuTimes t1, t2;
  unsigned long long idle1,idle2,noidle1,noidle2,total1,total2;
  double total_diff, idle_diff;

  if (read_cpu_times(&t1) < 0) return -1;
  usleep(CPU_SAMPLE_TIME);
  if (read_cpu_times(&t2) < 0) return -1;

  /* Classify CPU usage in idle and not idle*/
  idle1 = t1.idle + t1.iowait;
  idle2 = t2.idle + t2.iowait;

  noidle1 = t1.user + t1.nice + t1.system + t1.irq + t1.softirq + t1.steal;
  noidle2 = t2.user + t2.nice + t2.system + t2.irq + t2.softirq + t2.steal;

  total1 = idle1 + noidle1;
  total2 = idle2 + noidle2;

  total_diff = (double)(total2 - total1);
  idle_diff = (double)(idle2 - idle1);

  return (total_diff - idle_diff) / total_diff * 100.0;
}
