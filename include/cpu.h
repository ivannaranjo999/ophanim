#ifndef CPU_H
#define CPU_H

#define CPU_LINE_SIZE 256
#define CPU_SAMPLE_TIME 100000

/* For more info, see man proc_stat
 * user: time spent in user mode
 * nice: time spent in user mode with low priority
 * system: time spent in system mode
 * idle: time spent in the idle task
 * iowait: time waiting for I/O to complete
 * irq: time servicing interrupts
 * softirq: time servicing softirq
 * steal: time spent in other OS when running in a virtualized environment
 */
 
typedef struct {
  unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuTimes;

double get_cpu_usage();

#endif
